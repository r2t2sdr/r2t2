#include <QTime>
#include <QDebug>
#include <stdio.h>
#include <assert.h>
#include "config.h"
#include "audioQt.h"
#include "lib.h"

#define AUDIO_TIMEOUT		10
#define AUDIO_BUF_SIZE		8192

Audio::Audio(char* /*dev*/, char* /*mixerDev*/, char* /*mixerVol*/, char* /*mixerMic*/, int rate)  {
	tx = false;
	mute = false;
	noutput_items = AUDIO_BUF_SIZE;

	QAudioFormat format;
	format.setSampleRate(rate);
	format.setChannelCount(1);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

	QList<QAudioDeviceInfo> audioDevicesOut =  QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	QList<QAudioDeviceInfo> audioDevicesIn =  QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
	
	QAudioDeviceInfo infoIn  = audioDevicesIn.at(0);
	QAudioDeviceInfo infoOut = audioDevicesOut.at(0);

	if (!infoIn.isFormatSupported(format)) {
		qWarning()<<"AudioInput: default format not supported try to use nearest";
		format = infoIn.nearestFormat(format);
	}
	if (!infoOut.isFormatSupported(format)) {
		qWarning()<<"AudioInput: default format not supported try to use nearest";
		format = infoOut.nearestFormat(format);
	}
	audioOutput = new QAudioOutput(infoOut, format);
	audioInput = new QAudioInput(infoIn, format);
	audioInDev = NULL;
	audioOutDev = NULL;
	audioOutDev = audioOutput->start();
    periodSize = audioOutput->periodSize();
	
	mutex = new QMutex();
	timer = new QTimer(this);

    qDebug() << "period" << 1000*periodSize/(rate*2) << audioOutput->notifyInterval();

	timer->start(1000*periodSize/(rate*2)-1);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    audioRun = true;
}

Audio::~Audio() {
	timer->stop();
	audioOutput->stop();
	delete audioOutput;
	delete audioInput;
	delete timer;
}

void Audio::audioMute(bool m) {
	mute = m;
}

void Audio::setTX(bool m) {
	if (m)
		audioInDev = audioInput->start();
	else {
		audioInput->stop();
		audioInDev = NULL;
	}
}

void Audio::audioRX(QByteArray out) {
	if (!audioRun)
		return;

    if (audioOutBuf.size()>AUDIO_BUF_SIZE) {
        qDebug() << "# skip" << audioOutBuf.size();
        return;
    }
    mutex->lock();
	audioOutBuf.append(out);
    if (audioOutBuf.size() < AUDIO_BUF_SIZE/2) {
        audioOutBuf.append(out);
        qDebug() << "# insert" << out.size() << audioOutput->bytesFree();
    }
    mutex->unlock();
}

void Audio::timeout() {
    //qDebug() << audioOutput->state();
    mutex->lock();
    if (audioOutput->bytesFree()>=periodSize) {
        int len = audioOutDev->write(audioOutBuf);
        audioOutBuf.remove(0, len);
        //qDebug() <<  "write" << len << audioOutBuf.size() << audioOutput->bytesFree();
    }
    mutex->unlock();

	if (audioInDev) {
		QByteArray audioInData =  audioInDev->readAll();
		if (audioInData.size())
			emit audioTX(QByteArray(audioInData));
	}
}

void Audio::terminate() {
    audioRun = false;
}

void Audio::run() {
    while(audioRun) {
		msleep(200);
	}
}

void Audio::setVolume(int volume) {
    audioOutput->setVolume(1.0*volume/256);
}

void Audio::setMic(int volume) {
    audioInput->setVolume(1.0*volume/256);
}
