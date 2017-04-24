#include <sstream>
#include <QtCore/QCoreApplication>
#include <QTime>
#include <QDebug>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <google/protobuf/text_format.h>

#include "r2t2clientstandard.h"
#include "r2t2.pb.h"
#include "r2t2gui.pb.h"
#include "lib.h"
#include "types.h"
#include "config.h"

extern int debugLevel;

typedef struct _buffer {
    uint64_t sequence;
    uint16_t offset;
    uint16_t length;
    uint8_t data[500];
} QTRXBUF;


UdpReaderR2T2ClientStandard::UdpReaderR2T2ClientStandard(R2T2ClientStandard *parent) : parent(parent) { }

void UdpReaderR2T2ClientStandard::run() { 
    while (!isInterruptionRequested()) {
        parent->readR2T2ServerUDPData();
    }
    qDebug() << "reader terminated..";
}

UdpReaderR2T2ClientStandard::~UdpReaderR2T2ClientStandard() {
}

R2T2ClientStandard::R2T2ClientStandard(QSettings *settings, QString destAddr, quint16 port,
        QTcpSocket *tcpSocket, QString fileName, uint32_t sampleRate, uint32_t fftRate) 
    :  settings(settings), dstAddr(destAddr), port(port), tcpSocket(tcpSocket), sampleRate(sampleRate), fftRate(fftRate) {


        cmdFile = nullptr;
        qDebug() << QTime::currentTime().toString() <<  "R2T2ClientStandard created, Version " << VERSION  ", addr " << tcpSocket->peerName() << "," << tcpSocket->peerAddress().toString() <<  "port: " << tcpSocket->peerPort();

        if (fileName.isNull()) {
            input = new QTextStream(stdin);
            output = new QTextStream(stdout);
        } else {
            cmdFile = new QFile(fileName);
            if (!cmdFile->open(QFile::ReadOnly)) {
                qDebug() << "error open cmd file";
                QCoreApplication::exit(-1); 
            }
            input = new QTextStream(cmdFile);
            output = new QTextStream(stdout);
        }

#ifdef NATIVE_SOCKET
        serverSocket = openSocket(destAddr.toLocal8Bit().data(), port);
        udpReader = new UdpReaderR2T2ClientStandard(this);
        connect(udpReader, SIGNAL(sendAudio(std::shared_ptr<ProcessBuffer>)), this, SLOT(sendAudioData(std::shared_ptr<ProcessBuffer>)));
        connect(udpReader, SIGNAL(sendFFT(std::shared_ptr<ProcessBuffer>)), this, SLOT(sendFFTData(std::shared_ptr<ProcessBuffer>)));
        connect(udpReader, SIGNAL(sendCmd(int)), this, SLOT(sendCmd(int)));
#else
        serverSocket = new QUdpSocket(this);
        serverSocket->bind(0, QUdpSocket::ShareAddress);
        connect(serverSocket, SIGNAL(readyRead()), this, SLOT(readR2T2ServerUDPData()), Qt::DirectConnection );
#endif

        r2t2GuiMsg = new R2T2GuiProto::R2T2GuiMessage();
        r2t2GuiMsgAnswer = new R2T2GuiProto::R2T2GuiMessageAnswer();
        r2t2Msg = new R2T2Proto::R2T2Message();
        r2t2ServerMsg = new R2T2Proto::R2T2Message();

        rx = new RX("rx", sampleRate);
        tcpSink = new TCPSink("tcpSink", tcpSocket, fftRate);
        rx->connect(0, tcpSink, 0);
        rx->connect(1, tcpSink, 1);
        rx->connect(2, tcpSink, 2);

        txTimer = new QTimer(this);
        connect(txTimer, SIGNAL(timeout()), this, SLOT(sendTxDataRestart()));
        start();

        aliveTimer = new QTimer(this);
        connect(aliveTimer, SIGNAL(timeout()), this, SLOT(sendAlive()));
        aliveTimer->start(1000);

        rxBufPos = 0;
        fftBufPos = 0;
        spectrumReady = 0;

        udpReader->start();

        lastCommand = R2T2Proto::R2T2Message_Command_RXOPEN;
        r2t2ServerMsg->set_command(lastCommand);
        sendR2T2Msg();
        connected = true;	
    }

R2T2ClientStandard::~R2T2ClientStandard() {
    qDebug() << QTime::currentTime().toString() << "R2T2ClientStandard terminated " << tcpSocket->peerPort();

    mutex.lock();
    lastCommand = R2T2Proto::R2T2Message_Command_CLOSE;
    r2t2ServerMsg->set_command(lastCommand);
    sendR2T2Msg();
    mutex.unlock();

#ifdef NATIVE_SOCKET
    udpReader->requestInterruption();
    udpReader->wait();
	delete udpReader;
#endif

    delete rx;
    delete tcpSink;

    if (cmdFile) {
        cmdFile->close();
        delete cmdFile;
    }
#ifdef NATIVE_SOCKET
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
#else
    serverSocket->close();
    delete serverSocket;
#endif
    delete input;
    delete output;
    delete r2t2Msg;
    delete r2t2GuiMsg;
    delete r2t2GuiMsgAnswer;
    delete r2t2ServerMsg;
    aliveTimer->stop();
    delete aliveTimer;
    txTimer->stop();
    delete txTimer;
}

#ifdef NATIVE_SOCKET
int R2T2ClientStandard::openSocket (const char *host, unsigned short port) {

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock == -1) {
        perror("r2t2: socket open");
        return 0;
    }

    linger lngr;
    lngr.l_onoff  = 1;
    lngr.l_linger = 0;
    if(setsockopt(sock, SOL_SOCKET, SO_LINGER, &lngr, sizeof(linger)) == -1) {
        if( errno != ENOPROTOOPT) { 
            perror("r2t2: setsockopt"); 
            return 0;
        }
    }
    if(host != NULL ) {
        // Get the destination address
        struct addrinfo *ip_dst;
        struct addrinfo hints;
        memset( (void*)&hints, 0, sizeof(hints) );
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        char port_str[12];
        sprintf( port_str, "%d", port );

        if (getaddrinfo( host, port_str, &hints, &ip_dst )!=0) {
            perror("r2t2: socket getaddrinfo");
            return 0;
        }

        if(::connect(sock, ip_dst->ai_addr, ip_dst->ai_addrlen) == -1) {
            perror("r2t2: socket connect");
            return 0;
        }
        freeaddrinfo(ip_dst);
    }
    return sock;
}
#endif


// command from r2t2server
void R2T2ClientStandard::sendCmd(int cmd) {
	switch(cmd) {
        case R2T2Proto::R2T2Message_Command_ACK:
            if (lastCommand == R2T2Proto::R2T2Message_Command_RXOPEN) {
                connected = true;	
                r2t2GuiMsgAnswer->set_version(VERSION);
                sendR2T2GuiMsgAnswer();
            }
            lastCommand = R2T2Proto::R2T2Message_Command_NONE;
            break;

        case R2T2Proto::R2T2Message_Command_NACK:
            if (lastCommand == R2T2Proto::R2T2Message_Command_RXOPEN) {
                connected = false;	
            }
            lastCommand = R2T2Proto::R2T2Message_Command_NONE;
            break;
		default:
			assert(0);
	}
}

void R2T2ClientStandard::run() {
}

void R2T2ClientStandard::handleRadioFFTData(uint8_t* data, int len) {
    auto buf0 = std::make_shared<ProcessBuffer> (MAX_FFT_SIZE*2, typeid(cfloat_t));
    if (buf0) {
        buf0->setSize(len/sizeof(cfloat_t));
        memcpy(**buf0, data, len);
        emit udpReader->sendFFT(buf0);
    }
}

void R2T2ClientStandard::handleRadioRxData(uint8_t* data, int len) {

	len /= sizeof(cfloat_t);
	auto buf0 = std::make_shared<ProcessBuffer> (4096*4, typeid(cfloat_t));
	if (buf0) {
		buf0->setSize(len);
		memcpy(**buf0, data, len*sizeof(cfloat_t));
		emit udpReader->sendAudio(buf0);
	}
}

#ifdef NATIVE_SOCKET
int R2T2ClientStandard::getServerPacket(char *buf, int len) {

    fd_set readfds;
    timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(serverSocket, &readfds);
    int r = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
    if(r < 0) {
        perror("r2t2: select");
        return -1;
    } else if(r == 0 ) {  // timed out
        return -1;
    }

    r = recv(serverSocket, buf, len, 0); 

    if(r < 0) {
        if( errno ==  EAGAIN) { 
            return -1;
        } else {
            perror("r2t2 recv");
            return -1;
        }
    } 
    return r;
}
#endif

// read udp from r2r2 server 
void R2T2ClientStandard::readR2T2ServerUDPData() {
    uint8_t buf[4096+32];
    int len;

#ifdef NATIVE_SOCKET
    if ((len = getServerPacket((char*)buf, sizeof(buf))) > 0) {
#else
        QHostAddress senderAddr;
        quint16 senderPort;
        if (serverSocket->hasPendingDatagrams()) {
            len = serverSocket->readDatagram((char*)buf, sizeof(buf), &senderAddr, &senderPort);
#endif
            int pos = 0;

            if (buf[0]!='R' || buf[1]!='2') {
                qDebug("sync error: magic");
                return;
            }

            while(len > 0) {

                int pktLen = buf[pos+2]+(buf[pos+3]<<8);

                if (pktLen > len) {
                    qDebug() << "sync error: len" << pktLen << len;
                    return;
                }

                if (r2t2Msg->ParseFromArray(&buf[pos+4], pktLen)) {

					if (debugLevel == 5) {
						std::string formated;
						google::protobuf::TextFormat::PrintToString(*r2t2Msg, &formated);
						qDebug() << "Message from server " << len << "\n" << formated.data();
					}

                    if (r2t2Msg->has_rxdata()) 
                        handleRadioRxData((uint8_t*)(r2t2Msg->rxdata().data()), r2t2Msg->rxdata().size());

                    if (r2t2Msg->has_fftdata()) 
                        handleRadioFFTData((uint8_t*)(r2t2Msg->fftdata().data()), r2t2Msg->fftdata().size());

                    if (r2t2Msg->has_txdataack()) {
                        txAck -= r2t2Msg->txdataack() - lastAck;
                        lastAck = r2t2Msg->txdataack(); 
                    }

                    if (r2t2Msg->has_command()) {
						emit udpReader->sendCmd(r2t2Msg->command());
                    }
                    len -= pktLen+4;
                    pos += pktLen+4;
                }
            }
            r2t2Msg->Clear();
        }
    }


// read tcp from gui in dsp mode
void R2T2ClientStandard::readGuiTCPData() {
    uint8_t buf[2048];

    while (tcpSocket->bytesAvailable()) {
        int len = tcpSocket->read((char*)buf, sizeof(buf)-1);
        buf[len]=0;

        int pos = 0;

        mutex.lock();
        while(len > 0) {

            if (buf[0]!='R' || buf[1]!='2') {
                qDebug("sync error: magic");
                return;
            }

            int pktLen = buf[pos+2]+(buf[pos+3]<<8);

            if (pktLen > len) {
                qDebug() << "server sync error: len" << pktLen << len;
                return;
            }

            if (r2t2GuiMsg->ParseFromArray(&buf[pos+4], pktLen)) {
				if (debugLevel == 5) {
					std::string formated;
					google::protobuf::TextFormat::PrintToString(*r2t2GuiMsg, &formated);
					qDebug() << "Message from gui\n" << formated.data();
				}

                if (r2t2GuiMsg->has_rxfreq()) {
                    r2t2ServerMsg->set_rxfreq(r2t2GuiMsg->rxfreq());
                }
                if (r2t2GuiMsg->has_txfreq()) {
                    r2t2ServerMsg->set_txfreq(r2t2GuiMsg->txfreq());
                }
                if (r2t2GuiMsg->has_fftsize()) {
                    int size = rx->setFFTSize(r2t2GuiMsg->fftsize());
                    r2t2ServerMsg->set_fftsize(size);
                }
                if (r2t2GuiMsg->has_antenna()) {
                    curAntenna = std::min((int)r2t2GuiMsg->antenna(),1);
                    r2t2ServerMsg->set_antenna(curAntenna+1);
					r2t2GuiMsgAnswer->set_gain(gain[curAntenna]);
                }
                if (r2t2GuiMsg->has_mode())
                    rx->setMode((RXMode)r2t2GuiMsg->mode());

                if (r2t2GuiMsg->has_agc())
                    rx->setAGCDec(r2t2GuiMsg->agc());
                
                if (r2t2GuiMsg->has_filterlo()) {
                    filterLo = r2t2GuiMsg->filterlo();
                    rx->setFilter(filterLo, filterHi);
                }
                if (r2t2GuiMsg->has_filterhi()) {
                    filterHi = r2t2GuiMsg->filterhi();
                    rx->setFilter(filterLo, filterHi);
                }
                if (r2t2GuiMsg->has_notch()) {
                    rx->setNotch(r2t2GuiMsg->notch() > 0);
                }
                if (r2t2GuiMsg->has_fftrate()) {
                    uint32_t rate = r2t2GuiMsg->fftrate(); 
                    if (rate != fftRate && rate != sampleRate)
                        rate = fftRate;
                    tcpSink->setFFTRate(rate);
                    rx->setFFTAudio(rate == sampleRate);
                }
                if (r2t2GuiMsg->has_gain()) {
                    gain[curAntenna] = r2t2GuiMsg->gain();
                    int att = 0;
                    int preamp = 0;

                    if (gain[curAntenna]>0) {
                        preamp = gain[curAntenna];
                    } else {
                        att = -gain[curAntenna];
                    }

                    R2T2Proto::R2T2Message_Gain *msgGain = new R2T2Proto::R2T2Message_Gain();
                    msgGain->set_adc(curAntenna);
                    msgGain->set_gain(preamp);
                    r2t2ServerMsg->set_allocated_gain(msgGain);

                    R2T2Proto::R2T2Message_Gain *msgAtt = new R2T2Proto::R2T2Message_Gain();
                    msgAtt->set_adc(curAntenna);
                    msgAtt->set_gain(att);
                    r2t2ServerMsg->set_allocated_att(msgAtt);
                }

                if (r2t2GuiMsg->has_command()) {
                    switch (r2t2GuiMsg->command()) {
                        case R2T2GuiProto::R2T2GuiMessage_Command_STARTAUDIO:
                            lastCommand = R2T2Proto::R2T2Message_Command_STARTAUDIO;
                            r2t2ServerMsg->set_command(lastCommand);
                            r2t2ServerMsg->set_rxrate(sampleRate);
                            break;
                        case R2T2GuiProto::R2T2GuiMessage_Command_STOPAUDIO:
                            lastCommand = R2T2Proto::R2T2Message_Command_STOPAUDIO;
                            r2t2ServerMsg->set_command(lastCommand);
                            break;
                        case R2T2GuiProto::R2T2GuiMessage_Command_REQFFT:
                            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_REQFFT);
                            break;
                        default:
                            assert(0);
                    }
                }
                len -= pktLen+4;
                pos += pktLen+4;
            }
            sendR2T2Msg();
        }
        mutex.unlock();
        r2t2GuiMsg->Clear();
        sendR2T2GuiMsgAnswer();
    }
}

void R2T2ClientStandard::sendTxToneTestData() {
    int i;
    if (toneTestFreq==0) 
        return;

    double dph = 1.0*2*M_PI*toneTestFreq/txRate; 
    for (i=0;i<128;i++) {
        txBuf[i*2]   = (cos(p1)+cos(p2))*0.5;
        txBuf[i*2+1] = (sin(p1)+sin(p2))*0.5;
        p1 += dph;
        if (p1>2*M_PI) 
            p1-= 2*M_PI;
        p2 -= dph; 
        if (p2<-2*M_PI) 
            p2 += 2*M_PI;
    }
    mutex.lock();
    r2t2ServerMsg->set_txdata(txBuf, i*2*sizeof(float));
    txAck++;
    sendR2T2Msg();
    mutex.unlock();
}

// send alive to r2t2 server
void R2T2ClientStandard::sendAlive() {
    if (alive>0)
        alive--;
    if (alive==0 && connected) {
        mutex.lock();
        lastCommand = R2T2Proto::R2T2Message_Command_ALIVE;
        r2t2ServerMsg->set_command(lastCommand);
        sendR2T2Msg();
        mutex.unlock();
        alive = 5;
    }
}

void R2T2ClientStandard::sendTxDataRestart() {
}

void R2T2ClientStandard::sendAudioData(std::shared_ptr<ProcessBuffer> buf0) {
	rx->receive(buf0, 0, 0);
}

void R2T2ClientStandard::sendFFTData(std::shared_ptr<ProcessBuffer> buf0) {
	rx->receive(buf0, 1, 0);
}

void R2T2ClientStandard::disconnected() {
    delete this;
}

void R2T2ClientStandard::sendR2T2Msg() {

    if (r2t2ServerMsg->ByteSize() == 0) {
        mutex.unlock();
        return;
    }

    std::ostringstream out;
    r2t2ServerMsg->SerializeToOstream(&out);
	if (debugLevel == 5) {
		std::string formated;
		google::protobuf::TextFormat::PrintToString(*r2t2ServerMsg, &formated);
		qDebug() << "send message to server" << dstAddr << port << "\n" << formated.data();
	}

    strcpy((char*)outBuf, "R2");
    outBuf[2] = out.str().size() & 0xff;
    outBuf[3] = (out.str().size() >> 8) & 0xff;

    memcpy(outBuf+4, out.str().data(), out.str().size());
#ifdef NATIVE_SOCKET
    send(serverSocket, outBuf, out.str().size()+4, 0);
#else
    serverSocket->writeDatagram(QByteArray((const char*)outBuf, out.str().size()+4, dstAddr, port);
#endif

    r2t2ServerMsg->Clear();
    alive = 5;
}

void R2T2ClientStandard::sendR2T2GuiMsgAnswer() {

    mutex.lock();
    if (r2t2GuiMsgAnswer->ByteSize() == 0) {
        mutex.unlock();
        return;
    }

	std::ostringstream out;
	r2t2GuiMsgAnswer->SerializeToOstream(&out);
	if (debugLevel == 5) {
		std::string formated;
		google::protobuf::TextFormat::PrintToString(*r2t2GuiMsgAnswer, &formated);
		qDebug() << "send message to gui\n" << formated.data();
	}

    strcpy((char*)outBufGui, "R2");
    outBufGui[2] = out.str().size() & 0xff;
    outBufGui[3] = (out.str().size() >> 8) & 0xff;

    memcpy(outBufGui+4, out.str().data(), out.str().size());
    tcpSocket->write((const char*)outBufGui, out.str().size()+4);

    r2t2GuiMsgAnswer->Clear();
    mutex.unlock();
}
