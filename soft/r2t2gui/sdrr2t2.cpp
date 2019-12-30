#include <QByteArray>
#include <QFile>
#include <QTcpSocket>
#include <QHostAddress>
#include <unistd.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <google/protobuf/text_format.h>
#include <QTime>

#include "config.h"
#include "lib.h"
#include "sdrr2t2.h"
#include "assert.h"
#include "g711.h"

SdrR2T2::SdrR2T2 (QString ip, int port, QObject * /*parent*/) : ip(ip), port(port) {
    qDebug() << "sdr start " << ip << port;
}

SdrR2T2::~SdrR2T2() {
    tcpTimer->stop();
    fftTimer->stop();
    tcpSocket->disconnectFromHost();
    delete r2t2GuiMsg;
    delete r2t2GuiMsgAnswer;
    delete tcpSocket;
    delete tcpTimer;
    delete fftTimer;
}

void SdrR2T2::init() {
    r2t2GuiMsg = new R2T2GuiProto::R2T2GuiMessage();
    r2t2GuiMsgAnswer = new R2T2GuiProto::R2T2GuiMessageAnswer();

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readServerTCPData()));
    tcpTimer = new QTimer(this);
    tcpTimer->setSingleShot(true);
    connect(tcpTimer, SIGNAL(timeout()), this, SLOT(tcpTimeout()));
    fftTimer = new QTimer(this);
    connect(fftTimer, SIGNAL(timeout()), this, SLOT(fftTime()));

}

void SdrR2T2::setServer(QString serverIP, uint16_t serverPort) {
    ip = serverIP;
    port = serverPort;
}

void SdrR2T2::connectServer(bool con) {

    disconnectServer();

    if (con) {
        qDebug() << "connecting to " << ip << port;
        tcpSocket->connectToHost(QHostAddress(ip), port);
        tcpTimer->start(1000);
    }
}


void SdrR2T2::sendStartSeq() {
    qDebug() << "send start sequence";
    r2t2GuiMsg->Clear();
    r2t2GuiMsg->set_rxfreq(rxFreq);
    r2t2GuiMsg->set_fftsize(fftSize);
    r2t2GuiMsg->set_mode((R2T2GuiProto::R2T2GuiMessage_Mode)mode);
    r2t2GuiMsg->set_filterlo(filterLo);
    r2t2GuiMsg->set_filterhi(filterHi);
    r2t2GuiMsg->set_antenna(antenna);
    r2t2GuiMsg->set_agc((R2T2GuiProto::R2T2GuiMessage_AGC)agc);
    r2t2GuiMsg->set_notch(notch);
    r2t2GuiMsg->set_fftrate(fftRate);
    // not send, if not pressed
    // r2t2GuiMsg->set_gain(gain);
    r2t2GuiMsg->set_txfreq(txFreq);
    r2t2GuiMsg->set_noise(noise);
    r2t2GuiMsg->set_command(R2T2GuiProto::R2T2GuiMessage_Command_STARTAUDIO);
    sendR2T2GuiMsg();
    if (fftTimeRep > 0)
        fftTimer->start(fftTimeRep);
}

void SdrR2T2::connected() {
    tcpTimer->stop();
    fftTimer->stop();
    qDebug() << "connected";
    sendStartSeq();
    emit controlCommand(SRC_SDR, CMD_CONNECT, 1);
}

void SdrR2T2::error(QAbstractSocket::SocketError /*error*/) {
    qDebug() << "Socket Error";
    disconnected();
}

void SdrR2T2::disconnectServer() {
    if (tcpSocket) {
        tcpSocket->abort();
    }
}

void SdrR2T2::disconnected() {
    tcpTimer->stop();
    fftTimer->stop();
    tcp_buf_len = 0;
    qDebug() << "disconnected";
    emit controlCommand(SRC_SDR, CMD_CONNECT, 0);
}


void SdrR2T2::tcpTimeout() {
    qDebug() << "connect timeout";
    disconnected();
}

void SdrR2T2::readServerTCPData() {

    while (tcpSocket->bytesAvailable()) {
        tcp_buf_len += tcpSocket->read((char*)tcp_buf+tcp_buf_len, sizeof(tcp_buf)-tcp_buf_len);
    }

    if (tcp_buf_len >= 32000) {
        qDebug() << "tcp sync error: resetting";
        tcp_buf_len = 0;
    }

    int pos = 0;
    while(tcp_buf_len > 0) {

        if (tcp_buf[0]!='R' || tcp_buf[1]!='2') {
            qDebug("tcp sync error: resetting");
            tcp_buf_len = 0;
            break;
        }

        int pktLen = tcp_buf[pos+2]+(tcp_buf[pos+3]<<8);

        if (pktLen > tcp_buf_len) {
            // packet fragmented
            memcpy (tcp_buf, tcp_buf+pos, tcp_buf_len);
            break;
        }

        if (pktLen > 8192) {
            qDebug() << "invalid packet";
            tcp_buf_len = 0;
            break;
        }

        if (r2t2GuiMsgAnswer->ParseFromArray(&tcp_buf[pos+4], pktLen)) {

            if (debugLevel == 5) {
                std::string formated;
                google::protobuf::TextFormat::PrintToString(*r2t2GuiMsgAnswer, &formated);
                qDebug() << "message from client\n" << formated.data();
            }

            if (r2t2GuiMsgAnswer->has_rxdata()) {
                int l = r2t2GuiMsgAnswer->rxdata().size();
                const unsigned char *p = (const unsigned char*)r2t2GuiMsgAnswer->rxdata().data();

                if (l > arraysize(outBuf))
                    l = arraysize(outBuf);

                for (int i=0;i<l;i++)
                    outBuf[i]=(int16_t)alaw2linear(p[i]);

                emit audioRX(QByteArray((char*)outBuf, l*sizeof(uint16_t)));
            }

            if (r2t2GuiMsgAnswer->has_fftdata())
                emit fftData(QByteArray(r2t2GuiMsgAnswer->fftdata().data(), r2t2GuiMsgAnswer->fftdata().size()));

            if (r2t2GuiMsgAnswer->has_rssi()) {
                emit controlCommand(SRC_SDR, CMD_RSSI, (int)r2t2GuiMsgAnswer->rssi());
            }

#if 0
            if (r2t2GuiMsgAnswer->has_fftrate())
                emit controlCommand(SRC_SDR, CMD_FFT_SAMPLE_RATE, r2t2GuiMsgAnswer->fftrate());
#endif
            if (r2t2GuiMsgAnswer->has_gain())
                qDebug() << "Received gain:" << r2t2GuiMsgAnswer->gain() << "dB";
            //  for unknown reasons the gain value is way out of bound (not set properly @client?)
            //  emit controlCommand(SRC_SDR, CMD_PREAMP, r2t2GuiMsgAnswer->gain());

            if (r2t2GuiMsgAnswer->has_version())
                std::cout << "connected to client version " << r2t2GuiMsgAnswer->version() << std::endl;

            tcp_buf_len -= pktLen+4;
            pos += pktLen+4;
        } else {
            qDebug() << "parse error";
            tcp_buf_len = 0;
            break;
        }

    }
    r2t2GuiMsgAnswer->Clear();
}

void SdrR2T2::sendR2T2GuiMsg() {

    if (!tcpSocket)
        return;

    if (!tcpSocket->isOpen())
        return;

    if (r2t2GuiMsg->ByteSizeLong() == 0) {
        return;
    }

    std::ostringstream out;
    r2t2GuiMsg->SerializeToOstream(&out);
    if (debugLevel == 5) {
        std::string formated;
        google::protobuf::TextFormat::PrintToString(*r2t2GuiMsg, &formated);
        qDebug() << "message to client\n" << formated.data();
    }

    strcpy((char*)tcpOutBuf, "R2");
    tcpOutBuf[2] = out.str().size() & 0xff;
    tcpOutBuf[3] = (out.str().size() >> 8) & 0xff;

    memcpy(tcpOutBuf+4, out.str().data(), out.str().size());
    tcpSocket->write((const char*)tcpOutBuf, out.str().size()+4);

    // quint64 writtenBytes = tcpSocket->write((const char*)tcpOutBuf, out.str().size()+4);
    // qDebug("sending %d bytes", writtenBytes);

    r2t2GuiMsg->Clear();
}

void SdrR2T2::startRX() {
}

void SdrR2T2::stopRX() {
}

void SdrR2T2::setRXFreq(uint32_t f) {
    rxFreq = f;
    r2t2GuiMsg->set_rxfreq(rxFreq);
    sendR2T2GuiMsg();
}

void SdrR2T2::setTXFreq(uint32_t f) {
    txFreq = f;
    r2t2GuiMsg->set_txfreq(txFreq);
    sendR2T2GuiMsg();
}

void SdrR2T2::setSampleRate(int rate) {
    sampleRate = rate;
}

void SdrR2T2::setFFTRate(int rate) {
    fftRate = rate;
    r2t2GuiMsg->set_fftrate(fftRate);
    sendR2T2GuiMsg();
}

void SdrR2T2::setPtt(bool /*on*/) {
}

void SdrR2T2::setTXRate(int /*rate*/) {
}

void SdrR2T2::setTXLevel(int /*l*/) {
}

void SdrR2T2::setAttenuator(int n) {
    gain = n;
    r2t2GuiMsg->set_gain(gain);
    sendR2T2GuiMsg();
}

void SdrR2T2::setPresel(int /*n*/) {
}

void SdrR2T2::setAnt(int ant) {
    antenna = ant;
    r2t2GuiMsg->set_antenna(antenna);
    sendR2T2GuiMsg();
}

void SdrR2T2::setTxDelay(int /*txDelay*/) {
}

void SdrR2T2::setNBLevel(int /*level*/) {
}

void SdrR2T2::setFilter(int lo, int hi) {
    filterLo = lo;
    filterHi = hi;
    r2t2GuiMsg->set_filterlo(filterLo);
    r2t2GuiMsg->set_filterhi(filterHi);
    sendR2T2GuiMsg();
}

void SdrR2T2::setFFT(int time, int size) {
    fftSize = size;
    fftTimeRep = time;
    if (fftTimer->isActive()){
        fftTimer->stop();
        fftTimer->start(fftTimeRep);
    }
    r2t2GuiMsg->set_fftsize(fftSize);
    sendR2T2GuiMsg();
}

void SdrR2T2::setMode(int m) {
    mode = m;
    r2t2GuiMsg->set_mode((R2T2GuiProto::R2T2GuiMessage_Mode)mode);
    sendR2T2GuiMsg();
}

void SdrR2T2::setGain(int /* m */) {
}

void SdrR2T2::setAGC(int m) {
    agc = m;
    r2t2GuiMsg->set_agc((R2T2GuiProto::R2T2GuiMessage_AGC)agc);
    sendR2T2GuiMsg();
}

void SdrR2T2::setMicGain(double) {
}

void SdrR2T2::setVolume(double) {
}

void SdrR2T2::setToneTest(bool /*on*/, double /*f1*/, double /*level1*/, double /*f2*/, double /*level2*/) {
}

void SdrR2T2::setActive(bool /*on*/) {
}

void SdrR2T2::setAudioOff(bool /*on*/) {
}

void SdrR2T2::setNotch(int v) {
    notch = v;
    r2t2GuiMsg->set_notch(notch);
    sendR2T2GuiMsg();
}

void SdrR2T2::setSquelch(int /*v*/) {
}


void SdrR2T2::setComp(int) {
}

void SdrR2T2::selectPresel(int) {
}

void SdrR2T2::fftTime() {
    r2t2GuiMsg->set_command(R2T2GuiProto::R2T2GuiMessage_Command_REQFFT);
    sendR2T2GuiMsg();
}

void SdrR2T2::setRx(int) {
}

void SdrR2T2::setNoiseFilter(int v) {
    noise = v;
    r2t2GuiMsg->set_noise(noise);
    sendR2T2GuiMsg();
}
