#include <QTimer>
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
        void terminate();
		void setVolume(int volume);
		void setMic(int mic);
	private slots:
		void timeout();

	private:
        QAudioFormat format;
        QList<QAudioDeviceInfo> audioDevicesOut;
        QList<QAudioDeviceInfo> audioDevicesIn;
        QAudioOutput *audioOutput;
        QAudioInput *audioInput;
		QBuffer *audioOutBuffer, *audioInBuffer;
        QIODevice *audioOutDev, *audioInDev;
		QByteArray audioOutBuf;

        int noutput_items;
        bool tx,mute;
		QTimer *timer;
        bool audioRun;
		int minOutSize;
        int periodTimeMS;
};
