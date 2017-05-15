#include <QTimer>
#include <QBuffer>
#include <QMutex>
#include <QThread>
#include "config.h"
#include <alsa/asoundlib.h>

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
#ifdef USE_MIXER
		void setVolume(int volume);
		void setMic(int mic);
#endif
	private slots:
		void timeout();

	private:
		snd_pcm_t *PlaybackHandle;
		snd_pcm_t *RecordHandle;
		snd_mixer_t *mixerHandle;
		snd_mixer_elem_t* elemVol;
		snd_mixer_elem_t* elemMic;
		short outBuf[8000*2];
		short inBuf[8000];
		char audiodev[128];
		char mixerdev[32];
		char mixervol[32];
		char mixermic[32];
		int outBufPos, inBufPos;
		int noutput_items;
		bool mute;
		QTimer *timer;
		bool tx;
		long volumeMax, volumeMin;
		long micMax, micMin;
		int audioRate;
};
