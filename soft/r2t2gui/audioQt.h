#include <QBuffer>
#include <QThread>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioInput>

#include "config.h"

class Audio : public QObject  {
	Q_OBJECT
	public:
        Audio(char*, char*, char*, char*, int rate);
		~Audio();

	signals:
		void audioTX(QByteArray);
	public slots:
        void init();
		void audioRX(QByteArray);
		void setTX(bool);
		void audioMute(bool);
		void setVolume(int volume);
		void setMic(int mic);
    private slots:
        void writeAudioOut();
        void readAudioIn();
        void audioOutStateChanged(QAudio::State state);

	private:
        enum AudioRxState {
            AUDIO_RX_STATE_IDLE,
            AUDIO_RX_STATE_PLAY
        };
        AudioRxState audioRxState;
        QAudioFormat format;
        QList<QAudioDeviceInfo> audioDevicesOut;
        QList<QAudioDeviceInfo> audioDevicesIn;
        QAudioOutput *audioOutput;
        QAudioInput *audioInput;
        QIODevice *audioOutDev, *audioInDev;
        QByteArray audioOutBuf;

        int noutput_items;
        int packetSize;
        bool tx,mute;
        bool audioRun;
};
