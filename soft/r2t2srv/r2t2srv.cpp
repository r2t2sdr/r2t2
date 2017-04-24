#include <sstream>
#include <QTime>
#include <QDebug>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#include <google/protobuf/text_format.h>

#include "r2t2srv.h"
#include "lib.h"

#define SCALE_32 		(1.0/2147483647.0) 
#define SCALE_16 		(32767.0*0.9) 
#define MAX_INT32 		0xffffffff

extern int debugLevel;

UdpReader::UdpReader(R2T2Srv *parent) : parent(parent) { }

void UdpReader::run() { 
	while(1) {
		parent->readClientUDPData(); 
	}
}


R2T2Srv::R2T2Srv(uint32_t clk, quint16 port): port(port) {
    int i,idx=0;

	qDebug() << "R2T2Srv Version " << VERSION;

    r2t2 = new R2T2(clk);

#ifdef NATIVE_SOCKET
	udpReader = new UdpReader(this);
	udpReader->start();
#endif
	
	for (i=0;i<arraysize(client);i++)
		clearClient(i);
	for (i=0;i<MAX_RX_CLIENT;i++)
		client[idx++].usage = Usage::RX;
	for (i=0;i<MAX_TX_CLIENT;i++)
		client[idx++].usage = Usage::TX;

	for(i=0;i<2;i++) {
		gain[i]=1;
		att[i]=1;
	}

    r2t2Msg = new R2T2Proto::R2T2Message();
    r2t2ClientMsg = new R2T2Proto::R2T2Message();

#ifdef NATIVE_SOCKET
	clientSocket = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(clientSocket<0) {
		perror("create socket failed for iq_socket\n");
		exit(1);
	}

	linger lngr;
	lngr.l_onoff  = 1;
	lngr.l_linger = 0;
	if(setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, &lngr, sizeof(linger)) == -1) {
		if( errno != ENOPROTOOPT) { 
			perror("r2t2: setsockopt"); 
			exit(1);
		}
	}

	int iq_length=sizeof(iq_addr);
	memset(&iq_addr,0,iq_length);
	iq_addr.sin_family=AF_INET;
	iq_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	iq_addr.sin_port=htons(port);

	if(bind(clientSocket,(struct sockaddr*)&iq_addr,iq_length)<0) {
		perror("bind socket failed for iq socket");
		exit(1);
	}
#else
    clientSocket = new QUdpSocket(this);
    clientSocket->bind(port, QUdpSocket::ShareAddress);
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readClientUDPData()));
#endif

	conTimer = new QTimer(this); 
	connect(conTimer, SIGNAL(timeout()), this, SLOT(checkCon())); 
	conTimer->start(1000);

	start();
}

R2T2Srv::~R2T2Srv() {
    delete r2t2;
#ifdef NATIVE_SOCKET
	shutdown(clientSocket, SHUT_RDWR);
	close(clientSocket);
#else
    delete clientSocket;
#endif
	delete r2t2Msg;
	delete r2t2ClientMsg;
}

int R2T2Srv::allocClient(QHostAddress addr, quint16 port, Usage usage) {
    if (getClient(addr, port)>=0) 
        return -1;
    for (int i=0;i<arraysize(client);i++) {
        if (client[i].usage == usage) {
            if (client[i].port == 0) {
                client[i].addr = addr;
                client[i].port = port;
                return i;
            }
        }
    }
	return -1;
}

void R2T2Srv::clearClient(uint32_t n) {
	if (n>=arraysize(client))
		return;
	client[n].addr = 0;
	client[n].port = 0;
	client[n].timeout = 0;
	client[n].work = false;
	client[n].seq = 0;
	client[n].rxBufPos[0] = 0;
	client[n].rxBufPos[1] = 0;
	client[n].fftSize = 1024;
	client[n].curFFTSize = 0;
	client[n].fft = 0;
	client[n].curInput = 0;
}

int R2T2Srv::freeClient(QHostAddress addr, quint16 port) {
	int c = getClient(addr, port);
	if (c<0)
		return -1;

	clearClient(c);
	return c;
}

int R2T2Srv::getClient(QHostAddress addr, quint16 port) {
    for (int i=0;i<arraysize(client);i++) {
		if (client[i].port == port && client[i].addr == addr) {
			client[i].timeout = 0; 
			return i;
		}
    }
	return -1;
}

void R2T2Srv::checkCon() {
	for (int i=0;i<arraysize(client);i++) {
		if (client[i].port != 0) {
			client[i].timeout++;
			if (client[i].timeout > CONN_TIMEOUT) {
				mutex.lock();
				r2t2ClientMsg->set_command(R2T2Proto::R2T2Message_Command_TIMEOUT);
				sendR2T2Msg(client[i].addr, client[i].port);
				mutex.unlock();
				qDebug() << "timeout" << i << client[i].addr <<  client[i].port;
				clearClient(i);
			}
		}
	}
}

void R2T2Srv::sendAck(bool ok, QHostAddress &addr, quint16 port) {
	mutex.lock();
	if (ok)
		r2t2ClientMsg->set_command(R2T2Proto::R2T2Message_Command_ACK);
	else 
		r2t2ClientMsg->set_command(R2T2Proto::R2T2Message_Command_NACK);
	sendR2T2Msg(addr, port);
	mutex.unlock();
}

#ifdef NATIVE_SOCKET
int R2T2Srv::getServerPacket(char *buf, int len, QHostAddress *addr, quint16 *port) {

	fd_set readfds;
	timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(clientSocket, &readfds);
	int r = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
	if(r < 0) {
		perror("r2t2: select");
		return -1;
	} else if(r == 0 ) {  // timed out
		return -1;
	}

	sockaddr_in name;
	socklen_t l = sizeof(name);
	r = recvfrom(clientSocket, buf, len, 0, (struct sockaddr *)&name, (socklen_t *)&l); 

	if(r < 0) {
		if( errno ==  EAGAIN) { 
			return -1;
		} else {
			perror("r2t2 recv");
			return -1;
		}
	} 

	*port = ntohs(name.sin_port);
	*addr = QHostAddress(htonl(name.sin_addr.s_addr));

	return r;
}
#endif

void R2T2Srv::readClientUDPData() {
    uint8_t buf[64*1024];
	int len;
    QHostAddress sender;
    quint16 senderPort;

#ifdef NATIVE_SOCKET
	while ((len = getServerPacket((char*)buf, sizeof(buf), &sender, &senderPort)) > 0) {
#else
    while (clientSocket->hasPendingDatagrams()) {
        len = clientSocket->readDatagram((char*)buf, sizeof(buf), &sender, &senderPort);
#endif
		int pos = 0;
		int rx,tx;

		if (buf[0]!='R' || buf[1]!='2') {
			qDebug("sync error: magic");
			return;
		}

		while(len > 4) {

			int pktLen = buf[pos+2]+(buf[pos+3]<<8);
			if (pktLen > len) {
				qDebug() << "sync error: len" << pktLen << len;
				return;
			}


			if (r2t2Msg->ParseFromArray(&buf[pos+4], pktLen)) {

				if (debugLevel == 5) {
					std::string formated;
					google::protobuf::TextFormat::PrintToString(*r2t2Msg, &formated);
					qDebug() << "Message from client " << len << sender << senderPort << "\n" << formated.data();
				}

				if (r2t2Msg->has_rxfreq()) {
					rx = getClient(sender, senderPort);
					sendAck(rx>=0, sender, senderPort);
                    if (rx>=0)
                        r2t2->setRxFreq(rx, r2t2Msg->rxfreq());
				}
				if (r2t2Msg->has_txfreq()) {
					tx = getClient(sender, senderPort);
					sendAck(tx>=0, sender, senderPort);
                    if (tx>=0)
                        r2t2->setTxFreq(r2t2Msg->txfreq());
				}
				if (r2t2Msg->has_txrate()) {
					tx = getClient(sender, senderPort);
					sendAck(tx>=0, sender, senderPort);
                    if (tx>=0)
                        r2t2->setTxRate(r2t2Msg->txrate());
				}
				if (r2t2Msg->has_antenna()) {
					rx = getClient(sender, senderPort);
					sendAck(rx>=0, sender, senderPort);
                    if (rx>=0) {
                        r2t2->setInput(rx, r2t2Msg->antenna());
						client[rx].curInput = std::min((int)r2t2Msg->antenna()-1,1);
					}
				}
				if (r2t2Msg->has_gain()) {
					rx = getClient(sender, senderPort);
					sendAck(rx>=0, sender, senderPort);
                    if (rx>=0)
                        r2t2->setGain(r2t2Msg->gain().adc(), r2t2Msg->gain().gain());
					if (r2t2Msg->gain().adc() < 2)
						gain[r2t2Msg->gain().adc()] = pow(10, -r2t2Msg->gain().gain()/20.0);
				}
				if (r2t2Msg->has_att()) {
					rx = getClient(sender, senderPort);
					sendAck(rx>=0, sender, senderPort);
                    if (rx>=0)
                        r2t2->setAtt(r2t2Msg->att().adc(), r2t2Msg->att().gain());
					if (r2t2Msg->att().adc() < 2) 
						att[r2t2Msg->att().adc()] = pow(10, r2t2Msg->att().gain()/20.0);
				}
				if (r2t2Msg->has_fftsize()) {
					rx = getClient(sender, senderPort);
					sendAck(rx>=0, sender, senderPort);
                    if (rx>=0)
						client[rx].fftSize = r2t2Msg->fftsize();;
				}
				if (r2t2Msg->has_txdata()) {
					tx = getClient(sender, senderPort);
                    if (tx>=0) {
						r2t2->xmit((float*)(r2t2Msg->txdata().data()), r2t2Msg->txdata().size()/sizeof(float));
						mutex.lock();
                        r2t2ClientMsg->set_txdataack(++client[tx].seq);
						sendR2T2Msg(sender, senderPort);
						mutex.unlock();
					} else {
						mutex.lock();
						r2t2ClientMsg->set_command(R2T2Proto::R2T2Message_Command_NACK);
						sendR2T2Msg(sender, senderPort);
						mutex.unlock();
					}
				}
				if (r2t2Msg->has_command()) {
					switch(r2t2Msg->command()) {
						case R2T2Proto::R2T2Message_Command_RXOPEN:
							rx = allocClient(sender, senderPort, Usage::RX);
							sendAck(rx>=0, sender, senderPort);
							if (rx>=0) {
								mutex.lock();
								r2t2ClientMsg->set_rx(rx);
								sendR2T2Msg(sender, senderPort);
								mutex.unlock();
							}
							break;
						case R2T2Proto::R2T2Message_Command_TXOPEN:
							tx = allocClient(sender, senderPort, Usage::TX);
							sendAck(tx>=0, sender, senderPort);
							if (tx>=0) {
								mutex.lock();
								r2t2ClientMsg->set_tx(tx);
								sendR2T2Msg(sender, senderPort);
								mutex.unlock();
							}
							break;
						case R2T2Proto::R2T2Message_Command_CLOSE:
							rx = freeClient(sender, senderPort);
							sendAck(rx>=0, sender, senderPort);
							if (rx>=0) {
								mutex.lock();
								r2t2ClientMsg->set_rx(rx);
								sendR2T2Msg(sender, senderPort);
								mutex.unlock();
							}
							break;
						case R2T2Proto::R2T2Message_Command_STARTAUDIO:
							rx = getClient(sender, senderPort);
							sendAck(rx>=0, sender, senderPort);
							if (rx>=0)
								client[rx].work = true;
							break;
						case R2T2Proto::R2T2Message_Command_STOPAUDIO:
							rx = getClient(sender, senderPort);
							sendAck(rx>=0, sender, senderPort);
							if (rx>=0) 
								client[rx].work = false;
							break;
						case R2T2Proto::R2T2Message_Command_STARTFFT:
							rx = getClient(sender, senderPort);
							sendAck(rx>=0, sender, senderPort);
							if (rx>=0) 
								client[rx].fft = MAX_INT32 ;
							break;
						case R2T2Proto::R2T2Message_Command_STOPFFT:
							rx = getClient(sender, senderPort);
							sendAck(rx>=0, sender, senderPort);
							if (rx>=0) 
								client[rx].fft = 0; 
							break;
						case R2T2Proto::R2T2Message_Command_REQFFT:
							rx = getClient(sender, senderPort);
							sendAck(rx>=0, sender, senderPort);
							if (rx>=0 && client[rx].fft==0) { 
								client[rx].fft = 1; 
							}
							break;
						case R2T2Proto::R2T2Message_Command_ALIVE:
							rx = getClient(sender, senderPort);
							break;
						default:
							assert(0);
					}
				}
			}

			len -= pktLen+4;
			pos += pktLen+4;

			r2t2Msg->Clear();
		}
    }
}

void R2T2Srv::sendR2T2Msg(QHostAddress addr, quint16 port) {

	if (r2t2ClientMsg->ByteSize() == 0)
		return;

	std::ostringstream out;
	r2t2ClientMsg->SerializeToOstream(&out);
	if (debugLevel == 5) {
		std::string formated;
		google::protobuf::TextFormat::PrintToString(*r2t2ClientMsg, &formated);
		qDebug() << "Message to client " << addr << port << "\n" << formated.data();
	}

	strcpy((char*)outBuf, "R2");
	outBuf[2] = out.str().size() & 0xff;
	outBuf[3] = (out.str().size() >> 8) & 0xff;
	memcpy(outBuf+4, out.str().data(), out.str().size());

#ifdef NATIVE_SOCKET
	struct sockaddr_in clientAddr;
	int clientLength = sizeof(clientAddr);
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = htonl(addr.toIPv4Address());
	clientAddr.sin_port = htons(port);

	sendto(clientSocket, outBuf, out.str().size()+4, 0, (struct sockaddr*)&clientAddr, clientLength);
#else
	clientSocket->writeDatagram(QByteArray((char*)outBuf, out.str().size()+4), addr, port);
#endif

	r2t2ClientMsg->Clear();
}

void R2T2Srv::int2float(void* buf, int cnt, int rx) {
	float corr = SCALE_32*gain[client[rx].curInput]*att[client[rx].curInput];
	for (int i=0; i < cnt; i++)
		((float*)buf)[i]   = ((float) (((int32_t*)buf)[i] << 8)) * corr;
}


void R2T2Srv::handleAudioData(int32_t *out, int len) {
//	int maxInt = 0;
	for (int rx=0;rx<MAX_RX;rx++) {
		Client *c = &client[rx];
		if (c->work) {
			int pos = c->rxBufPos[TYPE_AUDIO];
			for (int i=0;i<len;i+=(MAX_RX*2)) {
				c->rxBuf[TYPE_AUDIO][pos]=out[rx*2+i];
				c->rxBuf[TYPE_AUDIO][pos+1]=out[rx*2+i+1];
				pos+=2;
#if 0
				if (rx==0) {
					int m = (out[rx*2+i] & 0xffffff) << 8;
					m = m/256;
					if (abs(m) > maxInt)
						maxInt = abs(m);
					m = (out[rx*2+i+1] & 0xffffff) << 8;
					m = m/256;
					if (abs(m) > maxInt)
						maxInt = abs(m);
				}
#endif

			}

			c->rxBufPos[TYPE_AUDIO]=pos;

			if (c->rxBufPos[TYPE_AUDIO] >= 384) {
				PDEBUG(MSG4, "rxSend %i %i", rx, c->rxBufPos[TYPE_AUDIO]);
				int2float(c->rxBuf[TYPE_AUDIO], c->rxBufPos[TYPE_AUDIO], rx);
				mutex.lock();
				r2t2ClientMsg->set_rxdata((float*)c->rxBuf[TYPE_AUDIO], c->rxBufPos[TYPE_AUDIO]*sizeof(float));
				sendR2T2Msg(c->addr, c->port);
				mutex.unlock();
				c->rxBufPos[TYPE_AUDIO] = 0;
			}
		}
	}
}

void R2T2Srv::handleFFTData(int32_t *out, int len) {
	for (int rx=0;rx<MAX_RX;rx++) {
		Client *c = &client[rx];
		if (c->fft > 0) {
			int pos = c->rxBufPos[TYPE_FFT];
			for (int i=0;i<len;i+=(MAX_RX*2)) {
				c->rxBuf[TYPE_FFT][pos]=out[rx*2+i];
				c->rxBuf[TYPE_FFT][pos+1]=out[rx*2+i+1];
				pos+=2;
			}
			c->rxBufPos[TYPE_FFT]=pos;

			if (c->rxBufPos[TYPE_FFT] >= 512) {
				PDEBUG(MSG4, "fftSend %i %i", rx, c->rxBufPos[TYPE_FFT]);
				int2float(c->rxBuf[TYPE_FFT], c->rxBufPos[TYPE_FFT], rx);
				mutex.lock();
				r2t2ClientMsg->set_fftdata((float*)c->rxBuf[TYPE_FFT], c->rxBufPos[TYPE_FFT]*sizeof(float));
				sendR2T2Msg(c->addr, c->port);
				mutex.unlock();

				if (c->fft != MAX_INT32) {
					c->curFFTSize += c->rxBufPos[TYPE_FFT]/2;
					if (c->curFFTSize >= c->fftSize) {
						c->curFFTSize = 0;
						c->fft--;
					}
				}
				c->rxBufPos[TYPE_FFT] = 0;
			}
		}
	}
}

void R2T2Srv::run() {
	int len, first, last, type;
	int32_t *out;

	// out is: I0,Q0,I1,Q1,...I7,Q7,I0,Q0...
	while(1) {
		type = r2t2->recv(&out, MAX_RX_SAMPLES, len, first, last);
		type--;
		switch(type) {
			case TYPE_AUDIO:
				handleAudioData(out, len);
				break;
			case TYPE_FFT:
				handleFFTData(out, len);
				break;
		}
	}
}

