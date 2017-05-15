#include <QByteArray>
#include <QFile>
#include <QTcpSocket>
#include <QHostAddress>
#include <unistd.h>
#include <stdint.h>
#include "config.h"
#include "lib.h"
#include "sdrqtradio.h"
#include "assert.h"
#include "g711.h"

SdrQtRadio::SdrQtRadio (QString ip, int port, QObject *parent) : ip(ip), port(port) {
    qDebug() << "initial connect" << ip << port+rx;
}

SdrQtRadio::~SdrQtRadio() {
    delete tcpSocket;
    delete timer;
    delete tcpTimer;
}

void SdrQtRadio::init() {
    tcpSocket = new QTcpSocket(this);
    // tcpSocket->connectToHost(QHostAddress(ip), port+rx);
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect (tcpSocket, SIGNAL(readyRead()), this, SLOT(readServerTCPData()));
    tcpTimer = new QTimer(this);
    tcpTimer->setSingleShot(true);
    connect(tcpTimer, SIGNAL(timeout()), this, SLOT(tcpTimeout()));
    // tcpTimer->start(1000);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(fftTime()));
}

void SdrQtRadio::setServer(QString serverIP, uint16_t serverPort) {
    ip = serverIP;
    port = serverPort;
}

void SdrQtRadio::connectServer(bool con) {
    if (con) {
        qDebug() << "connect" << ip << port+rx;
        if (conn) {
            tcpSocket->disconnectFromHost();
        }

        delete tcpSocket;
        tcpSocket = new QTcpSocket(this);
        connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
        connect (tcpSocket, SIGNAL(readyRead()), this, SLOT(readServerTCPData()));
        tcpSocket->connectToHost(QHostAddress(ip), port+rx);
        tcpTimer->start(1000);
    } else {
        startRx = true;
        timer->stop();
        tcpSocket->disconnectFromHost();
    }
}

void SdrQtRadio::sendStartSeq() {
    sendCmd("SetIQEnable false");
    sendCmd("setMode 0");
    sendCmd("SetIQEnable true");
    sendCmd("setClient QtRadio");
    sendCmd("setpwsmode 0");
    sendCmd("setEncoding 0");
    sendCmd("startAudioStream 800 8000 1 0");
    if (fftTimeRep > 0)
        timer->start(fftTimeRep);
}

void SdrQtRadio::connected() {
	qDebug() << "connected";
    tcpTimer->stop();
    inBuf.clear();
    conn = true;
    if (startRx) 
        sendStartSeq();

   setRXFreq(rxFreq); 
   setMode(mode);
   setFilter(filterLo, filterHi);
   emit controlCommand(SRC_SDR, CMD_CONNECT, 1); 
}

void SdrQtRadio::error(QAbstractSocket::SocketError /*error*/) {
    disconnected();
}

void SdrQtRadio::disconnected() {
    inBuf.clear();
    tcpTimer->stop();
    timer->stop();
    conn = false;
	qDebug() << "disconnected";
    emit controlCommand(SRC_SDR, CMD_CONNECT, 0); 
}

void SdrQtRadio::tcpTimeout() {
    qDebug() << "connect timeout";
    connectServer(false);
    disconnected();
}

void SdrQtRadio::handleSpectrum(int len) {
    // qDebug() << "fft" << len - FFT_BUFFER_HEADER_SIZE;
	emit fftData(inBuf.mid(FFT_BUFFER_HEADER_SIZE, len - FFT_BUFFER_HEADER_SIZE));
    // unsigned char in android ?
    char crssi = inBuf[6];
    int8_t rssi =(int8_t) crssi;
	emit controlCommand(SRC_SDR, CMD_RSSI, rssi); 
    uint32_t fftRate =  (uint8_t)inBuf[12]+((uint8_t)inBuf[11]<<8)+((uint8_t)inBuf[10]<<16)+((uint8_t)inBuf[9]<<24);
	emit controlCommand(SRC_SDR, CMD_FFT_SAMPLE_RATE, fftRate); 
    // uint32_t ifFreq =  (uint8_t)inBuf[14]+((uint8_t)inBuf[13]<<8);
    // qDebug() << "IF" << Rifreq;
}

void SdrQtRadio::handleAudio(int len) {
	len -= AUDIO_BUFFER_HEADER_SIZE;
    // qDebug() << "audio" << len;
	if (len > arraysize(outBuf))
		len = arraysize(outBuf);

	for (int i=0;i<len;i++)
		outBuf[i]=(int16_t)alaw2linear(inBuf[i + AUDIO_BUFFER_HEADER_SIZE]);

	emit audioRX(QByteArray((char*)outBuf, len*sizeof(uint16_t)));
}

void SdrQtRadio::readServerTCPData() {

	while (tcpSocket->bytesAvailable()) {
		inBuf += tcpSocket->readAll();
		if (inBuf.size() > AUDIO_BUFFER_HEADER_SIZE) {
			int headerSize = 0;
			if (inBuf[0]== (char)SPECTRUM_BUFFER)
				headerSize=FFT_BUFFER_HEADER_SIZE;
            else if (inBuf[0]== (char)AUDIO_BUFFER)
				headerSize=AUDIO_BUFFER_HEADER_SIZE;
            else {
                // qDebug() << inBuf;
                inBuf.clear();
            }
            int len = ((uint8_t)inBuf[3]<<8) + (uint8_t)inBuf[4] + headerSize;
			// qDebug() << "buflen" << len << inBuf.size() << headerSize;
			if (inBuf.size() >= len) {
				if (inBuf[0]==(char)SPECTRUM_BUFFER)
					handleSpectrum(len);
				if (inBuf[0]==(char)AUDIO_BUFFER)
					handleAudio(len);
				inBuf = inBuf.mid(len);
			}
		}
	}
}

void SdrQtRadio::sendCmd(QString cmd) {
    if (!conn)
        return;
    QByteArray a;
    a.append(cmd);
    while (a.size()<64)
        a.append((char)0);
    tcpSocket->write(a);
}

void SdrQtRadio::startRX() {
    if (conn)
        sendStartSeq();
    else
        startRx = true;
}

void SdrQtRadio::stopRX() {
    startRx = false;
	sendCmd("stopAudioStream");
}

void SdrQtRadio::setRXFreq(uint32_t f) {
    rxFreq = f;
    sendCmd(QString("setFrequency %1").arg(rxFreq));
}

void SdrQtRadio::setTXFreq(uint32_t /*f*/) {
}

void SdrQtRadio::setSampleRate(int /*rate*/) {
}

void SdrQtRadio::setFFTRate(int /*rate*/) {
}

void SdrQtRadio::setPtt(bool /*on*/) {
}

void SdrQtRadio::setTXRate(int /*rate*/) {
}

void SdrQtRadio::setTXLevel(int /*l*/) {
}

void SdrQtRadio::setAttenuator(int n) {
    if (n>=20) {
        sendCmd(QString("*activatepreamp 1"));
        sendCmd(QString("*setattenuator 0"));
    } else if (n>=10) {
        sendCmd(QString("*activatepreamp 1"));
        sendCmd(QString("*setattenuator 10"));
    } else {
        sendCmd(QString("*activatepreamp 0"));
        sendCmd(QString("*setattenuator %1").arg(-n));
    }
}

void SdrQtRadio::setPresel(int /*n*/) {
}

void SdrQtRadio::setAnt(int ant) {
    antenna = ant;
    sendCmd(QString("*selectantenna %1").arg(antenna));
}

void SdrQtRadio::setTxDelay(int /*txDelay*/) {
}

void SdrQtRadio::setNBLevel(int /*level*/) {
}

void SdrQtRadio::setFilter(int lo, int hi) {
    filterLo = lo;
    filterHi = hi;
    sendCmd(QString("setFilter %1 %2").arg(lo).arg(hi));
}

void SdrQtRadio::setFFT(int time, int size) {
    fftSize = size;
    fftTimeRep = time;
    if (fftTimeRep > 0)
        timer->start(fftTimeRep);
}

void SdrQtRadio::setMode(int m) {
    mode = m;
    sendCmd(QString("setMode %1").arg(mode));
}

void SdrQtRadio::setGain(int m) {
    sendCmd(QString("*activatepreamp %1").arg(m));
}

void SdrQtRadio::setAGC(int m) {
    sendCmd(QString("*SetAGC %1").arg(m));
}

void SdrQtRadio::setMicGain(double) {
}

void SdrQtRadio::setVolume(double) {
}

void SdrQtRadio::setToneTest(bool /*on*/, double /*f1*/, double /*level1*/, double /*f2*/, double /*level2*/) {
}

void SdrQtRadio::setActive(bool /*on*/) {
}

void SdrQtRadio::setAudioOff(bool /*on*/) {
}

void SdrQtRadio::setNotch(int /*v*/) {
}

void SdrQtRadio::setSquelch(int /*v*/) {
}

void SdrQtRadio::setComp(int) {
}

void SdrQtRadio::selectPresel(int) {
}

void SdrQtRadio::fftTime() {
    sendCmd(QString("getSpectrum %1").arg(fftSize));
}

void SdrQtRadio::setRx(int v) {
    rx = v;
}
