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

    format.setSampleRate(rate);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    audioDevicesOut =  QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    audioDevicesIn =  QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
}

void Audio::init(){

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
    audioRun = true;
}

Audio::~Audio() {
    if (audioOutput){
        audioOutput->stop();
        delete audioOutput;
    }
    if (audioInput){
        audioInput->stop();
        delete audioInput;
    }
}

void Audio::audioMute(bool m) {
	mute = m;
}

void Audio::setTX(bool /*m*/) {
// no tx, crash in win32 in audioInDev->readAll()
#if 0
	if (m)
		audioInDev = audioInput->start();
	else {
		audioInput->stop();
		audioInDev = NULL;
	}
#endif
}

void Audio::audioRX(QByteArray out) {
	if (!audioRun)
		return;

    if (audioOutput->bytesFree()>=audioOutput->periodSize()) {
        int len = audioOutDev->write(audioOutBuf);
        audioOutBuf.remove(0, len);
        // qWarning() <<  "write" << len << audioOutBuf.size() << audioOutput->bytesFree() << periodSize;
    }

    if (audioInDev) {
        QByteArray audioInData =  audioInDev->readAll();
        if (audioInData.size())
            emit audioTX(QByteArray(audioInData));
    }

    if (audioOutBuf.size()>AUDIO_BUF_SIZE) {
        qWarning() << "# skip" << audioOutBuf.size();
        return;
    }

	audioOutBuf.append(out);

//    if (audioOutBuf.size() < AUDIO_BUF_SIZE/2) {
//        audioOutBuf.append(out);
//        qWarning() << "# insert" << out.size() << audioOutput->bytesFree();
//    }

}

void Audio::timeout() {
}

void Audio::terminate() {
    audioRun = false;
}

void Audio::setVolume(int volume) {
    audioOutput->setVolume(1.0*volume/256);
}

void Audio::setMic(int volume) {
    audioInput->setVolume(1.0*volume/256);
}
