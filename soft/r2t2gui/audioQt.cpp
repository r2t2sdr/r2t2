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
    audioOutput->setBufferSize(AUDIO_BUF_SIZE);
    connect(audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(audioOutStateChanged(QAudio::State)));
    connect(audioOutput, SIGNAL(notify()), this, SLOT(writeAudioOut()));
    audioOutput->setNotifyInterval(AUDIO_TIMEOUT);
    audioOutDev = audioOutput->start();
    // start writing silence to the audio sink
    audioOutBuf = QByteArray(5000, '\x0');
    audioOutDev->write(audioOutBuf);

    // TBD start reading from Microphone
    audioInput = new QAudioInput(infoIn, format);
	audioInDev = NULL;
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

    if (audioOutBuf.length() + out.length() < AUDIO_BUF_SIZE - out.length()){
        audioOutBuf.append(out);
    }

    if (audioOutBuf.size()>AUDIO_BUF_SIZE) {
        qWarning() << "# audio buffer overflow" << audioOutBuf.size() << "bytes";
        return;
    }
}

void Audio::audioOutStateChanged(QAudio::State state)
{
    if (audioOutput->error() != QAudio::NoError){
        qDebug() << "audio output state:" << state << "error:" << audioOutput->error();
    }
}

void Audio::readAudioIn(){
        if (audioInDev) {
            QByteArray audioInData =  audioInDev->readAll();
            if (audioInData.size())
                emit audioTX(QByteArray(audioInData));
        }
}

void Audio::writeAudioOut()
{
    if (audioOutBuf.length() > 0){
        int len = audioOutDev->write(audioOutBuf);
        audioOutBuf.remove(0, len);
        return;
    }
    // write silence if audioOutbuf is empty to avoid underrun
    if (audioOutDev){
        QByteArray buf(2000, '\x0');
        audioOutDev->write(buf);
    }
}

void Audio::setVolume(int volume) {
    if (!audioOutput){
        qDebug() << "audioOutput not initialized";
        return;
    }
    if (audioOutput->state() == QAudio::ActiveState){
        audioOutput->setVolume(1.0*volume/256);
    }
}

void Audio::setMic(int volume) {
    if (!audioInput){
        qDebug() << "audioInput not initialized";
        return;
    }
    if (audioInput->state() == QAudio::ActiveState){
         audioInput->setVolume(1.0*volume/256);
    }
}
