#include <QTimer>
#include <QBuffer>
#include <QMutex>
#include <QThread>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioInput>

#include "config.h"

class Audio : public QThread  {
	Q_OBJECT
	public:
		Audio(char*, char*, char*, char*, int rate);
		~Audio();
		void run();

	signals:
		void audioTX(QByteArray);
	public slots:
		void audioRX(QByteArray);
		void setTX(bool);
		void audioMute(bool);
        void terminate();
		void setVolume(int volume);
		void setMic(int mic);
	private slots:
		void timeout();

	private:
		QAudioOutput *audioOutput;	
		QAudioInput *audioInput;	
		QBuffer *audioOutBuffer, *audioInBuffer;
		QIODevice *audioOutDev, *audioInDev;
		QByteArray audioOutBuf;

		short outBuf[8000*2];
		short inBuf[8000];
		int outBufPos, inBufPos;
		int noutput_items;
		bool tx,mute;
		QTimer *timer;
		QMutex *mutex;
        bool audioRun;
		int minOutSize;
        int periodSize;
};
