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


#include "r2t2clientqtradio.h"
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

class R2T2ClientQtRadio;

UdpReaderR2T2ClientQtRadio::UdpReaderR2T2ClientQtRadio(R2T2ClientQtRadio *parent) : parent(parent) { }

void UdpReaderR2T2ClientQtRadio::run() { 
    while (!isInterruptionRequested()) {
        parent->readR2T2ServerUDPData();
    }
    qDebug() << "reader terminated..";
}

UdpReaderR2T2ClientQtRadio::~UdpReaderR2T2ClientQtRadio() {
}

R2T2ClientQtRadio::R2T2ClientQtRadio(QSettings *settings, QString destAddr, quint16 port,
        QTcpSocket *tcpSocket, QString fileName, uint32_t sampleRate, uint32_t fftRate) 
    :  settings(settings), dstAddr(destAddr), port(port), tcpSocket(tcpSocket), sampleRate(sampleRate), fftRate(fftRate) {


        cmdFile = nullptr;
        qDebug() << QTime::currentTime().toString() <<  "R2T2ClientQtRadio created, Version " << VERSION  ", addr " << tcpSocket->peerName() << "," << tcpSocket->peerAddress().toString() <<  "port: " << tcpSocket->peerPort();

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
        udpReader = new UdpReaderR2T2ClientQtRadio(this);
        connect(udpReader, SIGNAL(sendCmd(int)), this, SLOT(sendCmd(int)));
#else
        serverSocket = new QUdpSocket(this);
        serverSocket->bind(0, QUdpSocket::ShareAddress);
        connect(serverSocket, SIGNAL(readyRead()), this, SLOT(readR2T2ServerUDPData()), Qt::DirectConnection );
#endif

        // for qt radio
#ifdef NATIVE_SOCKET_CLIENT
        clientSocket = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if(clientSocket<0) {
            perror("create socket failed for iq_socket\n");
            exit(1);
        }
#else
        clientSocket = new QUdpSocket(this);
        clientSocket->bind(11000, QUdpSocket::ShareAddress);
#endif

        r2t2Msg = new R2T2Proto::R2T2Message();
        r2t2ServerMsg = new R2T2Proto::R2T2Message();

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

R2T2ClientQtRadio::~R2T2ClientQtRadio() {
    qDebug() << QTime::currentTime().toString() << "R2T2ClientQtRadio terminated " << tcpSocket->peerPort();

    mutex.lock();
    r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_CLOSE);
    mutex.unlock();
    sendR2T2Msg();

#ifdef NATIVE_SOCKET
    udpReader->requestInterruption();
    udpReader->wait();
	delete udpReader;
#endif

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
    
#ifdef NATIVE_SOCKET_CLIENT
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
#else
    clientSocket->close();
    delete clientSocket;
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
int R2T2ClientQtRadio::openSocket (const char *host, unsigned short port) {

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
void R2T2ClientQtRadio::sendCmd(int /*cmd*/) {
}

void R2T2ClientQtRadio::run() {
}

void R2T2ClientQtRadio::readClientTCPData() {
    uint8_t buf[2048];
    QString line, cmd1, cmd2;
    qint64 arg1, arg2;

    while (tcpSocket->bytesAvailable()) {
        int len = tcpSocket->read((char*)buf, sizeof(buf)-1);

        buf[len]=0;
        QStringList s = QString((char*)buf).split(' ');

        cmd1 = s.value(0);
        cmd2 = s.value(1);
        arg1 = s.value(1).toLongLong();
        arg2 = s.value(2).toLongLong();

        //qDebug() << "cmd: " << cmd1 << cmd2;

        mutex.lock();
        if (cmd1 == "attach") {
			qDebug() << cmd1;
			lastCommand = R2T2Proto::R2T2Message_Command_RXOPEN;
            r2t2ServerMsg->set_command(lastCommand);
            rxBufPos = 0;
            fftBufPos = 0;
        } else if (cmd1 == "detach") {
            qtClientPort = 0;
            qtClientFFTPort = 0;
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_CLOSE);
            sendClientTcpResp(QString("OK %1").arg(sampleRate));
            connected = false;	
        } else if (cmd1 == "frequency") {
            r2t2ServerMsg->set_rxfreq(arg1);
            sendClientTcpResp(QString("OK"));
        } else if (cmd1 == "start" && cmd2 == "fft") {
            qtClientFFTPort = arg2;
            qtClientFFTAddr = tcpSocket->peerAddress(); 
            sendClientTcpResp(QString("OK"));
        } else if (cmd1 == "start" && cmd2 == "iq") {
            r2t2ServerMsg->set_rxrate(sampleRate);
            r2t2ServerMsg->set_fftrate(fftRate);
			lastCommand = R2T2Proto::R2T2Message_Command_STARTAUDIO;
            r2t2ServerMsg->set_command(lastCommand);
            qtClientPort = arg2;
            qtClientAddr = tcpSocket->peerAddress(); 
        } else if (cmd1 == "stop" && cmd2 == "iq") {
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_STOPAUDIO);
            qtClientPort = 0;
            qtClientFFTPort = 0;
            sendClientTcpResp(QString("OK"));
        } else if (cmd1 == "hardware?") {
            sendClientTcpResp(QString("OK HiQSDR"));
        } else if (cmd1 == "getserial?") {
            sendClientTcpResp(QString("R2T2"));
        } else if (cmd1 == "activatepreamp") {
            R2T2Proto::R2T2Message_Gain *msgGain = new R2T2Proto::R2T2Message_Gain();
            msgGain->set_adc(curAntenna);
            msgGain->set_gain(arg1*20);
            r2t2ServerMsg->set_allocated_gain(msgGain);
            sendClientTcpResp(QString("OK"));
        } else if (cmd1 == "setattenuator") {
            R2T2Proto::R2T2Message_Gain *msgAtt = new R2T2Proto::R2T2Message_Gain();
            msgAtt->set_adc(curAntenna);
            msgAtt->set_gain(arg1);
            r2t2ServerMsg->set_allocated_att(msgAtt);
            sendClientTcpResp(QString("OK"));
        } else if (cmd1 == "selectantenna") {
            curAntenna = std::min((int)arg1,1);
            r2t2ServerMsg->set_antenna(arg1+1);
            sendClientTcpResp(QString("OK"));
        } else {
            qDebug() << "unknown command: " << cmd1 << cmd2;
            sendClientTcpResp(QString("ERROR unknown command"));
        }
        mutex.unlock();
        sendR2T2Msg();
    }
}

void R2T2ClientQtRadio::handleQtRadioFFTData(uint8_t* data, int len) {
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

void R2T2ClientQtRadio::handleQtRadioRxData(uint8_t* data, int len) {
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

#ifdef NATIVE_SOCKET
int R2T2ClientQtRadio::getServerPacket(char *buf, int len) {

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
void R2T2ClientQtRadio::readR2T2ServerUDPData() {
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
                        handleQtRadioRxData((uint8_t*)(r2t2Msg->rxdata().data()), r2t2Msg->rxdata().size());
                    }

                    if (r2t2Msg->has_fftdata()) {
                        assert(0);
                        handleQtRadioFFTData((uint8_t*)(r2t2Msg->fftdata().data()), r2t2Msg->fftdata().size());
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

void R2T2ClientQtRadio::sendTxToneTestData() {
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
void R2T2ClientQtRadio::sendAlive() {
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

void R2T2ClientQtRadio::sendQResp(QString q, QString s, QString sep) {
    QString answer = q+sep+s;
    sendClientTcpResp(QString("4%1%2").arg(answer.size()).arg(answer));
    // qDebug() << "resp: " << QString("4%1%2").arg(answer.size()).arg(answer);
}

void R2T2ClientQtRadio::sendClientTcpResp(QString str) {
    QByteArray a;
    a.append(str);
    tcpSocket->write(a);
}

void R2T2ClientQtRadio::sendTxDataRestart() {
    qDebug() << "txRestart";
}

void R2T2ClientQtRadio::disconnected() {
    delete this;
}

void R2T2ClientQtRadio::sendR2T2Msg() {

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

