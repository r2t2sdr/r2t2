#include <QTimer>
#include <QBuffer>
#include <QMutex>
#include <QThread>

#include <windef.h>
#include <winbase.h>
#include <mmsystem.h>

#include "config.h"


#define AUDIO_IN_BUFSIZE	2048
#define AUDIO_OUT_BUFSIZE	8000

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
#ifdef USE_MIXER
		void setVolume(int volume);
		void setMic(int mic);
#endif
	private slots:
		void timeout();

	private:
		HWAVEIN      hWaveIn;
		HWAVEOUT     hWaveOut;
		PBYTE        pBuffer1, pBuffer2, pSaveBuffer, pNewBuffer;
		PWAVEHDR     pWaveHdr1, pWaveHdr2, pWaveHdrRx;
		short outBuf[AUDIO_OUT_BUFSIZE];
		char audiodev[128];
		char mixerdev[32];
		char mixervol[32];
		char mixermic[32];
		int outBufPos;
		int lastOutPos;
		int noutput_items;
		bool mute;
		QTimer *timer;
		QMutex *mutex;
		bool tx;
		long volumeMax, volumeMin;
		long micMax, micMin;
		int stopTx;
};
