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

#include "r2t2clientdspqtradio.h"
#include "r2t2.pb.h"
#include "lib.h"
#include "types.h"
#include "config.h"


typedef struct _buffer {
    uint64_t sequence;
    uint16_t offset;
    uint16_t length;
    uint8_t data[500];
} QTRXBUF;


UdpReaderR2T2ClientDSPQtRadio::UdpReaderR2T2ClientDSPQtRadio(R2T2ClientDSPQtRadio *parent) : parent(parent) { }

void UdpReaderR2T2ClientDSPQtRadio::run() { 
    while (!isInterruptionRequested()) {
        parent->readR2T2ServerUDPData();
    }
    qDebug() << "reader terminated..";
}

UdpReaderR2T2ClientDSPQtRadio::~UdpReaderR2T2ClientDSPQtRadio() {
}

R2T2ClientDSPQtRadio::R2T2ClientDSPQtRadio(QSettings *settings, QString destAddr, quint16 port,
        QTcpSocket *tcpSocket, QString fileName, uint32_t sampleRate, uint32_t fftRate) 
    :  settings(settings), dstAddr(destAddr), port(port), tcpSocket(tcpSocket), sampleRate(sampleRate), fftRate(fftRate) {


        cmdFile = nullptr;
        qDebug() << QTime::currentTime().toString() <<  "R2T2ClientDSPQtRadio created, Version " << VERSION  ", addr " << tcpSocket->peerName() << "," << tcpSocket->peerAddress().toString() <<  "port: " << tcpSocket->peerPort();

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
        udpReader = new UdpReaderR2T2ClientDSPQtRadio(this);
        connect(udpReader, SIGNAL(sendAudio(std::shared_ptr<ProcessBuffer>)), this, SLOT(sendAudioData(std::shared_ptr<ProcessBuffer>)));
        connect(udpReader, SIGNAL(sendFFT(std::shared_ptr<ProcessBuffer>)), this, SLOT(sendFFTData(std::shared_ptr<ProcessBuffer>)));
        connect(udpReader, SIGNAL(sendCmd(int)), this, SLOT(sendCmd(int)));
#else
        serverSocket = new QUdpSocket(this);
        serverSocket->bind(0, QUdpSocket::ShareAddress);
        connect(serverSocket, SIGNAL(readyRead()), this, SLOT(readR2T2ServerUDPData()), Qt::DirectConnection );
#endif

        r2t2Msg = new R2T2Proto::R2T2Message();
        r2t2ServerMsg = new R2T2Proto::R2T2Message();

        rx = new RX("rx", sampleRate);
        tcpSink = new TCPSinkQtRadio("tcpSink", tcpSocket, fftRate);
        rx->connect(0, tcpSink, 0);
        rx->connect(1, tcpSink, 1);
        rx->connect(2, tcpSink, 2);

        //lastCommand = R2T2Proto::R2T2Message_Command_RXOPEN;
        r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_RXOPEN);
        sendR2T2Msg();
        connected = true;	

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
    }

R2T2ClientDSPQtRadio::~R2T2ClientDSPQtRadio() {
    qDebug() << QTime::currentTime().toString() << "R2T2ClientDSPQtRadio terminated " << tcpSocket->peerPort();

    mutex.lock();
    r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_CLOSE);
    mutex.unlock();
    sendR2T2Msg();

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
    delete r2t2ServerMsg;
    aliveTimer->stop();
    delete aliveTimer;
    txTimer->stop();
    delete txTimer;
}

#ifdef NATIVE_SOCKET
int R2T2ClientDSPQtRadio::openSocket (const char *host, unsigned short port) {

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
void R2T2ClientDSPQtRadio::sendCmd(int cmd) {
	switch(cmd) {
        case R2T2Proto::R2T2Message_Command_ACK:
            if (lastCommand == R2T2Proto::R2T2Message_Command_RXOPEN) {
                sendClientTcpResp(QString("OK %1").arg(sampleRate));
                connected = true;	
                qDebug() << "open ack";
            }
            if (lastCommand == R2T2Proto::R2T2Message_Command_STARTAUDIO) {
                sendClientTcpResp(QString("OK"));
                qDebug() << "start audio";
            }
            lastCommand = R2T2Proto::R2T2Message_Command_NONE;
            break;

        case R2T2Proto::R2T2Message_Command_NACK:
            if (lastCommand == R2T2Proto::R2T2Message_Command_RXOPEN) {
                qDebug() << "open nack";
                sendClientTcpResp(QString("ERROR").arg(sampleRate));
                connected = false;	
            }
            if (lastCommand == R2T2Proto::R2T2Message_Command_STARTAUDIO) {
                sendClientTcpResp(QString("ERROR"));
                qDebug() << "start audio nack";
            }
            lastCommand = R2T2Proto::R2T2Message_Command_NONE;
            break;
		default:
			assert(0);
	}
}

void R2T2ClientDSPQtRadio::run() {
}

void R2T2ClientDSPQtRadio::handleQtRadioDspFFTData(uint8_t* data, int len) {
    auto buf0 = std::make_shared<ProcessBuffer> (MAX_FFT_SIZE*2, typeid(cfloat_t));
    if (buf0) {
        buf0->setSize(len/sizeof(cfloat_t));
        memcpy(**buf0, data, len);
        emit udpReader->sendFFT(buf0);
    }
}

void R2T2ClientDSPQtRadio::handleQtRadioFFTData(uint8_t* data, int len) {
    QTRXBUF buf;
    int offset = 0;

    if (fftBufPos >= (int)sizeof(fftBuf)) {
        assert(0);
        fftBufPos = 0;
    }

    if (qtClientFFTPort == 0)
        return;

    memcpy(fftBuf+fftBufPos, data, len);
    fftBufPos+=len;

    // TODO remove
    if (fftBufPos>= (int)sizeof(fftBuf)) {
        int len = fftBufPos;
        //qDebug() << "send fft" << len << qtClientPort;
        while(len>0) {
            int bufLen = std::min(len, 500);
            buf.sequence = qtFFTSequence++;
            buf.offset = offset;
            buf.length = bufLen;
            memcpy(buf.data, &fftBuf[offset], bufLen);
#ifdef NATIVE_SOCKET_CLIENT
            struct sockaddr_in clientAddr;
            int clientLength = sizeof(clientAddr);
            clientAddr.sin_family = AF_INET;
            clientAddr.sin_addr.s_addr = htonl(qtClientFFTAddr.toIPv4Address());
            clientAddr.sin_port = htons(qtClientFFTPort);
            sendto(clientSocket, (const char*)&buf, HEADER_SIZE+bufLen, 0, (struct sockaddr*)&clientAddr, clientLength);
#else
            clientSocket->writeDatagram((const char*)&buf, HEADER_SIZE+bufLen, qtClientFFTAddr, qtClientFFTPort);
#endif

            len-=bufLen;
            offset+=bufLen;
        }
        //qtFFTSequence++;
        fftBufPos = 0;
    }
}

void R2T2ClientDSPQtRadio::handleQtRadioRxData(uint8_t* data, int len) {
    QTRXBUF buf;
    int offset = 0;
    float* fdata = (float*)data;

    struct sockaddr_in clientAddr;
    int clientLength = sizeof(clientAddr);
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(qtClientAddr.toIPv4Address());
    clientAddr.sin_port = htons(qtClientPort);

    if (qtClientPort == 0)
        return;

    for (int i=0;i<(int)(len/sizeof(float)/2);i++) {
        rxBuf[rxBufPos+1024] = fdata[i*2]; 
        rxBuf[rxBufPos]      = fdata[i*2+1]; 
        rxBufPos++;
        if (rxBufPos >= 1024) {
            int len = rxBufPos*sizeof(float)*2;
            //qDebug() << "send iq" << len << qtClientPort;
            while(len>0) {
                int bufLen = std::min(len, 500);
                buf.sequence = qtSequence;
                buf.offset = offset;
                buf.length = bufLen;
                memcpy(buf.data, &rxBuf[offset/sizeof(float)], bufLen);
#ifdef NATIVE_SOCKET_CLIENT
                sendto(clientSocket, (const char*)&buf, HEADER_SIZE+bufLen, 0, (struct sockaddr*)&clientAddr, clientLength);
#else
                clientSocket->writeDatagram((const char*)&buf, HEADER_SIZE+bufLen, qtClientAddr, qtClientPort);
#endif
                len-=bufLen;
                offset+=bufLen;
            }
            qtSequence++;
            rxBufPos = 0;
        }
    }
}

void R2T2ClientDSPQtRadio::handleQtRadioDspRxData(uint8_t* data, int len) {

	len /= sizeof(cfloat_t);
	auto buf0 = std::make_shared<ProcessBuffer> (4096*4, typeid(cfloat_t));
	if (buf0) {
		buf0->setSize(len);
		memcpy(**buf0, data, len*sizeof(cfloat_t));
		emit udpReader->sendAudio(buf0);
	}
}

#ifdef NATIVE_SOCKET
int R2T2ClientDSPQtRadio::getServerPacket(char *buf, int len) {

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
void R2T2ClientDSPQtRadio::readR2T2ServerUDPData() {
    uint8_t buf[4096+32];
    int len;

#ifdef NATIVE_SOCKET
    if ((len = getServerPacket((char*)buf, sizeof(buf))) > 0) {
#else
        QHostAddress sender;
        quint16 senderPort;
        if (serverSocket->hasPendingDatagrams()) {
            len = serverSocket->readDatagram((char*)buf, sizeof(buf), &sender, &senderPort);
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
                    if (r2t2Msg->has_rxdata()) {
                        handleQtRadioDspRxData((uint8_t*)(r2t2Msg->rxdata().data()), r2t2Msg->rxdata().size());
                    }

                    if (r2t2Msg->has_fftdata()) {
                        handleQtRadioDspFFTData((uint8_t*)(r2t2Msg->fftdata().data()), r2t2Msg->fftdata().size());
                    }

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
void R2T2ClientDSPQtRadio::readDspTCPData() {
    uint8_t buf[2048];
    QString line, cmd1, cmd2;
    qint64 arg1, arg2;

    while (tcpSocket->bytesAvailable()) {
        int len = tcpSocket->read((char*)buf, sizeof(buf)-1);
        buf[len]=0;

        int pos = 0;
        while (pos < len) {
            while (pos < len && buf[pos]==0) pos++;
            if (pos >= len)
                break;

            QStringList s = QString((char*)buf+pos).split(' ');
            pos += QString((char*)buf+pos).size();

            cmd1 = s.value(0);
            cmd2 = s.value(1);
            arg1 = s.value(1).toLongLong();
            arg2 = s.value(2).toLongLong();

            mutex.lock();
            if (cmd1 == "q-version") {
                sendQResp(cmd1, QString(VERSION));
            } else if (cmd1 == "q-server") {
                sendQResp(cmd1, QString("DL2STG N"));
            } else if (cmd1 == "q-master") {
                sendQResp(cmd1, QString("master"));
            } else if (cmd1 == "q-loffset") {
                sendQResp(cmd1, QString("0.00"));
            } else if (cmd1 == "q-procol3") {
                sendQResp(cmd1, QString("Y"));
            } else if (cmd1 == "*hardware?") {
                sendQResp(cmd1, QString("OK HiQSDR"), QString(" "));
            } else if (cmd1 == "*selectantenna") {
                curAntenna = std::min((int)arg1,1);
                r2t2ServerMsg->set_antenna(arg1+1);
                sendQResp(cmd1, QString("OK"), QString(" "));
            } else if (cmd1 == "*setattenuator") {
                R2T2Proto::R2T2Message_Gain *msgAtt = new R2T2Proto::R2T2Message_Gain();
                msgAtt->set_adc(curAntenna);
                msgAtt->set_gain(arg1);
                r2t2ServerMsg->set_allocated_att(msgAtt);
                sendQResp(cmd1, QString("OK"), QString(" "));
            } else if (cmd1 == "*activatepreamp") {
                R2T2Proto::R2T2Message_Gain *msgGain = new R2T2Proto::R2T2Message_Gain();
                msgGain->set_adc(curAntenna);
                msgGain->set_gain(arg1*20);
                r2t2ServerMsg->set_allocated_gain(msgGain);
                sendQResp(cmd1, QString("OK"), QString(" "));
            } else if (cmd1 == "SetIQEnable") {
            } else if (cmd1 == "SetClient") {
            } else if (cmd1 == "startAudioStream") {
                r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_STARTAUDIO);
                r2t2ServerMsg->set_rxrate(sampleRate);
                r2t2ServerMsg->set_antenna(1);
            } else if (cmd1 == "setFilter") {
                rx->setFilter(arg1, arg2);
            } else if (cmd1 == "setFrequency") {
                r2t2ServerMsg->set_rxfreq(arg1);
			} else if (cmd1 == "SetAGC") {
				rx->setAGCDec(arg1);
			} else if (cmd1 == "getSpectrum") {
                int size = rx->setFFTSize(arg1);
                r2t2ServerMsg->set_fftsize(size);
				r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_REQFFT);
			} else if (cmd1 == "setMode") {
                rx->setMode((RXMode)arg1);
			} else if (cmd1 == "zoom") {
            } else {
                ; //    qDebug() << "unknown command: " << cmd1 << cmd2;
                //    sendClientTcpResp(QString("ERROR unknown command"));
            }
            mutex.unlock();
            sendR2T2Msg();
        }
    }
}

void R2T2ClientDSPQtRadio::sendTxToneTestData() {
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
    mutex.unlock();
    txAck++;
    qDebug() << "txData" << txAck;
    sendR2T2Msg();
}

// send alive to r2t2 server
void R2T2ClientDSPQtRadio::sendAlive() {
    if (alive>0)
        alive--;
    if (alive==0 && connected) {
        mutex.lock();
        r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_ALIVE);
        mutex.unlock();
        sendR2T2Msg();
        alive = 5;
    }
}

void R2T2ClientDSPQtRadio::sendQResp(QString q, QString s, QString sep) {
    QString answer = q+sep+s;
    sendClientTcpResp(QString("4%1%2").arg(answer.size()).arg(answer));
    // qDebug() << "resp: " << QString("4%1%2").arg(answer.size()).arg(answer);
}

void R2T2ClientDSPQtRadio::sendClientTcpResp(QString str) {
    QByteArray a;
    a.append(str);
    tcpSocket->write(a);
}

void R2T2ClientDSPQtRadio::sendTxDataRestart() {
    qDebug() << "txRestart";
}

void R2T2ClientDSPQtRadio::sendAudioData(std::shared_ptr<ProcessBuffer> buf0) {
	rx->receive(buf0, 0, 0);
}

void R2T2ClientDSPQtRadio::sendFFTData(std::shared_ptr<ProcessBuffer> buf0) {
	rx->receive(buf0, 1, 0);
}

void R2T2ClientDSPQtRadio::disconnected() {
    delete this;
}

void R2T2ClientDSPQtRadio::sendR2T2Msg() {

    mutex.lock();
    if (r2t2ServerMsg->ByteSize() == 0) {
        mutex.unlock();
        return;
    }

    std::ostringstream out;
    r2t2ServerMsg->SerializeToOstream(&out);
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
    mutex.unlock();
}

