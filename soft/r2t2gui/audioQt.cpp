#include <QTime>
#include <QDebug>
#include <QThread>
#include <stdio.h>
#include <assert.h>
#include "config.h"
#include "audioQt.h"
#include "lib.h"

#define AUDIO_TIMEOUT		10
#define AUDIO_BUF_SIZE		8192
#define AUDIO_TIMING_CORR   32

Audio::Audio(char* /*dev*/, char* /*mixerDev*/, char* /*mixerVol*/, char* /*mixerMic*/, int rate)  {
    tx = false;
    mute = false;
    noutput_items = AUDIO_BUF_SIZE;
    packetSize = 0;

    format.setSampleRate(rate);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    audioDevicesOut =  QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    audioDevicesIn =  QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

    init();
}

void Audio::init(){
    QAudioDeviceInfo infoIn  = audioDevicesIn.at(0);
    QAudioDeviceInfo infoOut = audioDevicesOut.at(0);

#if 0
    int n=0;
    qDebug() << "\navaiable audio output devices:";
    foreach (const QAudioDeviceInfo &deviceInfo, audioDevicesOut) {
        qDebug() << n++ << "Device name: " << deviceInfo.deviceName();
        foreach (const int freq, deviceInfo.supportedSampleRates())
            qDebug() << freq;
    }
    qDebug() << "\nusing audio output :" << infoOut.deviceName();

    qDebug() << "\navaiable audio input devices:";
    foreach (const QAudioDeviceInfo &deviceInfo, audioDevicesIn) {
        qDebug() << "Device name: " << deviceInfo.deviceName();
    }
    qDebug() << "\nusing audio in:" << infoIn.deviceName();
#endif

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
#ifndef ANDROID
    connect(audioOutput, SIGNAL(notify()), this, SLOT(writeAudioOut()));
#endif
    audioOutput->setNotifyInterval(AUDIO_TIMEOUT);
    audioOutDev = audioOutput->start();
    // start writing silence to the audio sink
    audioOutBuf = QByteArray(AUDIO_BUF_SIZE/2, '\x0');
    audioOutDev->write(audioOutBuf);

    // TBD start reading from Microphone
    audioInput = new QAudioInput(infoIn, format);
    audioInDev = NULL;
    audioRxState = AUDIO_RX_STATE_IDLE;
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

    packetSize = out.length();
    // qWarning() << "append" << packetSize << audioOutBuf.size();

    if (audioRxState == AUDIO_RX_STATE_PLAY) {
        if (audioOutBuf.length() < packetSize/2) {
            // insert samples to correct sample rates
            out.append(out.right(AUDIO_TIMING_CORR));
            qWarning() << "insert at" << audioOutBuf.length();
        } else if (audioOutBuf.length() >packetSize*2) {
            // remove samples to correct sample rates
            out.chop(AUDIO_TIMING_CORR);
            qWarning() << "skip at" << audioOutBuf.length();
        }
    }
        
    audioOutBuf.append(out);

    if (audioOutBuf.size()>AUDIO_BUF_SIZE) {
        qWarning() << "# audio buffer overflow" << audioOutBuf.size() << "bytes";
        audioOutBuf.truncate(AUDIO_BUF_SIZE);
        return;
    }
#ifdef ANDROID
    int len = audioOutDev->write(audioOutBuf);
    audioOutBuf.remove(0, len);
#endif

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
    int len;

    if (!audioOutDev)
        return;

    switch (audioRxState) {
        case AUDIO_RX_STATE_IDLE:
            if (packetSize > 0 && audioOutBuf.length() >= packetSize*2) {
                qDebug() << "state play" << audioOutBuf.length();
                int len = audioOutDev->write(audioOutBuf);
                audioOutBuf.remove(0, len);
                audioRxState = AUDIO_RX_STATE_PLAY;
                break;
            }
            // write silence if audioOutbuf is empty to avoid underrun
            audioOutDev->write(QByteArray(AUDIO_BUF_SIZE/4, '\x0'));
            break;

        case AUDIO_RX_STATE_PLAY:
            if (audioOutBuf.length() == 0) {
                audioOutDev->write(QByteArray(AUDIO_BUF_SIZE/4, '\x0'));
                packetSize = 0;
                qDebug() << "state idle";
                audioRxState = AUDIO_RX_STATE_IDLE;
                break;
            }
            len = audioOutDev->write(audioOutBuf);
            audioOutBuf.remove(0, len);
            break;
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
