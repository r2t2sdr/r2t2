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


#include "r2t2clientconsole.h"
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


UdpReaderR2T2ClientConsole::UdpReaderR2T2ClientConsole (R2T2ClientConsole *parent) : parent(parent) { }

void UdpReaderR2T2ClientConsole ::run() { 
    while (!isInterruptionRequested()) {
        parent->readR2T2ServerUDPData();
    }
    qDebug() << "reader terminated..";
}

UdpReaderR2T2ClientConsole ::~UdpReaderR2T2ClientConsole () {
}

R2T2ClientConsole::R2T2ClientConsole(QSettings *settings, QString destAddr, quint16 port,
        QTcpSocket *tcpSocket, QString fileName, uint32_t sampleRate, uint32_t fftRate) 
    :  settings(settings), dstAddr(destAddr), port(port), tcpSocket(tcpSocket), sampleRate(sampleRate), fftRate(fftRate) {


        cmdFile = nullptr;
        qDebug() << "R2T2ClientConsole Version " << VERSION << endl;

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
        udpReader = new UdpReaderR2T2ClientConsole (this);
        connect(udpReader, SIGNAL(sendCmd(int)), this, SLOT(sendCmd(int)));
#else
        serverSocket = new QUdpSocket(this);
        serverSocket->bind(0, QUdpSocket::ShareAddress);
        connect(serverSocket, SIGNAL(readyRead()), this, SLOT(readR2T2ServerUDPData()), Qt::DirectConnection );
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

R2T2ClientConsole::~R2T2ClientConsole() {
    qDebug() << QTime::currentTime().toString() << "R2T2ClientConsole terminated " << tcpSocket->peerPort();

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
int R2T2ClientConsole::openSocket (const char *host, unsigned short port) {

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

void R2T2ClientConsole::usage() {
    printf ("usage:\n\n");

    printf ("att <gain>   : set attentuator (range 0 .. -30dB)\n");
    printf ("close        : close all\n");
    printf ("gain <gain>  : set gain (range -10 .. 30 dB)\n");
    printf ("help         : view this help\n");
    printf ("init         : init hardware\n");
    printf ("quit         : close program\n");
    printf ("rxa          : set rx antenna (0: I, 1: Q, 2: IQ\n");
    printf ("rxf <freq>   : set rx freq\n");
    printf ("rxopen       : open next available rx\n");
    printf ("rxr <rate>   : set rx sample rate\n");
    printf ("start        : start rx audio transfer\n");
    printf ("stop         : stop rx audio transfer\n");
    printf ("startfft     : start rx fft data transfer\n");
    printf ("reqfft       : req one rx fft data transfer\n");
    printf ("sfft <size>  : set fft size, used from reqfft\n");
    printf ("stopfft      : stop rx fft data  transfer\n");
    printf ("tt <freq>    : start two tone test with delta freq <freq>\n");
    printf ("txf <freq>   : set tx freq\n");
    printf ("txopen       : open tx\n");
    printf ("txr <rate>   : set tx sample rate\n");
}


// command from r2t2server
void R2T2ClientConsole::sendCmd(int cmd) {
	switch(cmd) {
		case R2T2Proto::R2T2Message_Command_ACK:
            *output << QLatin1String("*"); 
            output->flush();
			break;
		case R2T2Proto::R2T2Message_Command_NACK:
            *output << QLatin1String("?"); 
            output->flush();
			break;
		case R2T2Proto::R2T2Message_Command_TIMEOUT:
            *output << QLatin1String("timeout"); 
            output->flush();
            connected = false;	
			break;
		default:
			assert(0);
	}
}

// input in console mode
void R2T2ClientConsole::readConsole() {
    QString line, cmd;
    qint64 arg1,arg2;

    *output << QLatin1String("$ "); 
    output->flush();

    while (input->readLineInto(&line)) {

        QStringList s = line.split(' ');

        cmd = s.value(0);
        arg1 = s.value(1).toLongLong();
        arg2 = s.value(2).toLongLong();

        mutex.lock();
        if (cmd == "rxopen") {
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_RXOPEN);
            connected = true;	
            lastAck = 0;
        }else if (cmd == "txopen") {
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_TXOPEN);
            connected = true;	
        }else if (cmd == "close") {
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_CLOSE);
            connected = false;	
        }else if (cmd == "start") {
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_STARTAUDIO);
        }else if (cmd == "startfft") {
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_STARTFFT);
        }else if (cmd == "reqfft") {
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_REQFFT);
        }else if (cmd == "stop") {
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_STOPAUDIO);
        }else if (cmd == "stopfft") {
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_STOPFFT);
        }else if (cmd == "init") {
            r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_REINIT);
        }else if (cmd == "sfft") {
            r2t2ServerMsg->set_fftsize(arg1);
        }else if (cmd == "rxf") {
            r2t2ServerMsg->set_rxfreq(arg1);
        } else if (cmd == "txf") {
            r2t2ServerMsg->set_txfreq(arg1);
        }else if (cmd == "rxr") {
            r2t2ServerMsg->set_rxrate(arg1);
        }else if (cmd == "txr") {
            r2t2ServerMsg->set_txrate(arg1);
            txRate = arg1;
        }else if (cmd == "rxa") {
            r2t2ServerMsg->set_antenna(arg1);
        }else if (cmd == "gain") {
            R2T2Proto::R2T2Message_Gain *msgGain = new R2T2Proto::R2T2Message_Gain();
            msgGain->set_adc(arg1);
            msgGain->set_gain(arg2);
            r2t2ServerMsg->set_allocated_gain(msgGain);
        }else if (cmd == "att") {
            R2T2Proto::R2T2Message_Gain *msgAtt = new R2T2Proto::R2T2Message_Gain();
            msgAtt->set_adc(arg1);
            msgAtt->set_gain(arg2);
            r2t2ServerMsg->set_allocated_att(msgAtt);
        } else if (cmd == "tt") {
            txAck = 0;
            toneTestFreq = arg1;
            mutex.unlock();
            for (int i=0;i<3;i++)
                sendTxToneTestData();
        } else if (cmd == "help") {
            usage();
        } else if (cmd == "quit") {
            break;
        } else {
            *output << QLatin1String("unknown command\n"); 
        }
        mutex.unlock();

        sendR2T2Msg();
        *output << QLatin1String("$ "); 
        output->flush();
    }
    QCoreApplication::quit(); 
}

void R2T2ClientConsole::run() {
    readConsole();
}


#ifdef NATIVE_SOCKET
int R2T2ClientConsole::getServerPacket(char *buf, int len) {

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
void R2T2ClientConsole::readR2T2ServerUDPData() {
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

                    if (r2t2Msg->has_rx()) {
                        *output << QLatin1String("rx:") << r2t2Msg->rx(); 
                        output->flush();
                    }

                    if (r2t2Msg->has_rxdata()) {
                        qDebug() << "rxData" << r2t2Msg->rxdata().size();
                    }

                    if (r2t2Msg->has_fftdata()) {
                        qDebug() << "fftData" << r2t2Msg->fftdata().size();
                    }

                    if (r2t2Msg->has_txdataack()) {
                        txAck -= r2t2Msg->txdataack() - lastAck;
                        lastAck = r2t2Msg->txdataack(); 
                        *output << QLatin1String("tx data ack") << lastAck << txAck; 
                        output->flush();
                        sendTxToneTestData();
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


void R2T2ClientConsole::sendTxToneTestData() {
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
void R2T2ClientConsole::sendAlive() {
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

void R2T2ClientConsole::sendTxDataRestart() {
    qDebug() << "txRestart";
}

void R2T2ClientConsole::disconnected() {
    delete this;
}

void R2T2ClientConsole::sendR2T2Msg() {

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

