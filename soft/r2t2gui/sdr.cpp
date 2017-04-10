#include <QByteArray>
#include <QFile>
#include <QTcpSocket>
#include <QHostAddress>
#include <unistd.h>
#include <stdint.h>
#include "config.h"
#include "lib.h"
#include "sdr.h"
#include "assert.h"
#include "g711.h"

Sdr::Sdr (QString ip, int port) : ip(ip), port(port) {
    tcpSocket = new QTcpSocket(this);
	tcpSocket->connectToHost(QHostAddress(ip), R2T2_SERVER_PORT);
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect (tcpSocket, SIGNAL(readyRead()), this, SLOT(readServerTCPData()));
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(fftTime()));
}

Sdr::~Sdr() {
    delete tcpSocket;
	delete timer;
}

void Sdr::connectServer(QString serverIP, uint16_t serverPort) {
    ip = serverIP;
    port = serverPort;
    qDebug() << "connect" << ip << port;
    if (conn)
        tcpSocket->disconnectFromHost();
    while (conn) 
        msleep(100);
    tcpSocket->connectToHost(QHostAddress(ip), port);
}

void Sdr::disconnectServer() {
    startRx = true;
    tcpSocket->disconnectFromHost();
}

void Sdr::sendStartSeq() {
    sendCmd("SetIQEnable false");
    sendCmd("setMode 0");
    sendCmd("SetIQEnable true");
    sendCmd("setClient QtRadio");
    sendCmd("setpwsmode 0");
    sendCmd("setEncoding 0");
    sendCmd("startAudioStream 800 8000 1 0");
    qDebug() << "start audio";
}

void Sdr::connected() {
	qDebug() << "connected";
    inBuf.clear();
    conn = true;
    if (startRx) 
        sendStartSeq();

   setRXFreq(rxFreq); 
   setMode(mode);
   setFilter(filterLo, filterHi);
}

void Sdr::disconnected() {
    inBuf.clear();
    conn = false;
	qDebug() << "disconnected";
}

void Sdr::handleSpectrum(int len) {
    // qDebug() << "fft" << len - FFT_BUFFER_HEADER_SIZE;
	emit fftData(inBuf.mid(FFT_BUFFER_HEADER_SIZE, len - FFT_BUFFER_HEADER_SIZE));
	emit controlCommand(SRC_SDR, CMD_RSSI, inBuf[6]); 
    uint32_t fftRate =  (uint8_t)inBuf[12]+((uint8_t)inBuf[11]<<8)+((uint8_t)inBuf[10]<<16)+((uint8_t)inBuf[9]<<24);
	emit controlCommand(SRC_SDR, CMD_FFT_SAMPLE_RATE, fftRate); 


}

void Sdr::handleAudio(int len) {
	len -= AUDIO_BUFFER_HEADER_SIZE;
    // qDebug() << "audio" << len;
	if (len > arraysize(outBuf))
		len = arraysize(outBuf);

	for (int i=0;i<len;i++)
		outBuf[i]=(int16_t)alaw2linear(inBuf[i + AUDIO_BUFFER_HEADER_SIZE]);

	emit audioRX(QByteArray((char*)outBuf, len*sizeof(uint16_t)));
}

void Sdr::readServerTCPData() {

	while (tcpSocket->bytesAvailable()) {
		inBuf += tcpSocket->readAll();
		if (inBuf.size() > AUDIO_BUFFER_HEADER_SIZE) {
			int headerSize = 0;
			if (inBuf[0]== (char)SPECTRUM_BUFFER)
				headerSize=FFT_BUFFER_HEADER_SIZE;
            else if (inBuf[0]== (char)AUDIO_BUFFER)
				headerSize=AUDIO_BUFFER_HEADER_SIZE;
            else {
                qDebug() << inBuf;
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

void Sdr::sendCmd(QString cmd) {
    if (!conn)
        return;
    QByteArray a;
    a.append(cmd);
    while (a.size()<64)
        a.append((char)0);
    tcpSocket->write(a);
}

void Sdr::startRX() {
    if (conn)
        sendStartSeq();
    else
        startRx = true;
}

void Sdr::stopRX() {
    startRx = false;
	sendCmd("stopAudioStream");
}

void Sdr::setRXFreq(uint32_t f) {
    rxFreq = f;
    sendCmd(QString("setFrequency %1").arg(rxFreq));
}

void Sdr::setTXFreq(uint32_t /*f*/) {
}

void Sdr::setSampleRate(int /*rate*/) {
}


void Sdr::setPtt(bool /*on*/) {
}

void Sdr::setTXRate(int /*rate*/) {
}

void Sdr::setTXLevel(int /*l*/) {
}

void Sdr::setAttenuator(int n) {
    qDebug() << "att" << n;
    if (n==20) {
        sendCmd(QString("*activatepreamp 1"));
        sendCmd(QString("*setattenuator 0"));
    } else if (n==10) {
        sendCmd(QString("*activatepreamp 1"));
        sendCmd(QString("*setattenuator 10"));
    } else {
        sendCmd(QString("*activatepreamp 0"));
        sendCmd(QString("*setattenuator %1").arg(-n));
    }
}

void Sdr::setPresel(int /*n*/) {
}

void Sdr::setAnt(int ant) {
    antenna = ant;
    sendCmd(QString("*selectantenna %1").arg(antenna));
}

void Sdr::setTxDelay(int /*txDelay*/) {
}

void Sdr::setNBLevel(int /*level*/) {
}

void Sdr::setFilter(int lo, int hi) {
    filterLo = lo;
    filterHi = hi;
    sendCmd(QString("setFilter %1 %2").arg(lo).arg(hi));
}

void Sdr::setFFT(int time, int size) {
    fftSize = size;
    timer->start(time);
    qDebug() << "time" << time << size;
}

void Sdr::setMode(int m) {
    mode = m;
    sendCmd(QString("setMode %1").arg(mode));
}

void Sdr::setGain(int m) {
    qDebug() << "preamp" << m;
    sendCmd(QString("*activatepreamp %1").arg(0));
}

void Sdr::setAGCDec(int m) {
    sendCmd(QString("*SetAGC %1").arg(m));
}

void Sdr::setMicGain(double) {
}

void Sdr::setVolume(double) {
}

void Sdr::setToneTest(bool /*on*/, double /*f1*/, double /*level1*/, double /*f2*/, double /*level2*/) {
}

void Sdr::setActive(bool /*on*/) {
}

void Sdr::setAudioOff(bool /*on*/) {
}

void Sdr::setNotch(int /*v*/) {
}

void Sdr::setSquelch(int /*v*/) {
}

void Sdr::terminate() {
	sdrRun = false;
}

void Sdr::run() {
    while(sdrRun) {
		msleep(200);
    }
}

void Sdr::setComp(int) {
}

void Sdr::selectPresel(int) {
}

void Sdr::fftTime() {
    sendCmd(QString("getSpectrum %1").arg(fftSize));
}
