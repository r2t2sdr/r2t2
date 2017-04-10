#include <QTime>
#include <QDebug>
#include <stdio.h>
#include <assert.h>
#include "config.h"
#include "audio.h"
#include "lib.h"

#define AUDIO_TIMEOUT		40
#define AUDIO_BUF_SIZE 		8192

Audio::Audio(char* dev, char* mixerDev, char* mixerVol, char* mixerMic, int rate)  {

	int err;
	tx = false;
	mute = false;
	noutput_items = AUDIO_BUF_SIZE;
	audioRate = rate;

	memset(audiodev,0,sizeof(audiodev));
	memset(mixerdev,0,sizeof(mixerdev));
	memset(mixervol,0,sizeof(mixervol));
	memset(mixermic,0,sizeof(mixermic));

	if (dev)
		strncpy(audiodev, dev, strnlen(dev, sizeof(audiodev)));
	else
		strncpy(audiodev, DEFAULT_AUDIO_DEV, sizeof(DEFAULT_AUDIO_DEV));

	if (mixerDev)
		strncpy(mixerdev, mixerDev, strnlen(mixerDev, sizeof(mixerdev)));
	else
		strncpy(mixerdev, DEFAULT_MIXER_DEV, sizeof(DEFAULT_MIXER_DEV));

	if (mixerVol)
		strncpy(mixervol, mixerVol, strnlen(mixerVol, sizeof(mixervol)));
	else
		strncpy(mixervol, DEFAULT_MIXER_VOL, sizeof(DEFAULT_MIXER_VOL));

	if (mixerMic)
		strncpy(mixermic, mixerMic, strnlen(mixerMic, sizeof(mixermic)));
	else
		strncpy(mixermic, DEFAULT_MIXER_MIC, sizeof(DEFAULT_MIXER_MIC));


#ifdef USE_MIXER
	snd_mixer_selem_id_t *sid;

	snd_mixer_open(&mixerHandle, 0);
	snd_mixer_attach(mixerHandle, mixerdev);
	snd_mixer_selem_register(mixerHandle, NULL, NULL);
	snd_mixer_load(mixerHandle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, mixervol);
	elemVol = snd_mixer_find_selem(mixerHandle, sid);
	snd_mixer_selem_get_playback_volume_range(elemVol, &volumeMin, &volumeMax);

	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, mixermic);
	elemMic = snd_mixer_find_selem(mixerHandle, sid);
	snd_mixer_selem_get_playback_volume_range(elemMic, &micMin, &micMax);
#endif

	if ((err = snd_pcm_open(&PlaybackHandle, audiodev, SND_PCM_STREAM_PLAYBACK , SND_PCM_NONBLOCK)) < 0)
		printf("Can't open audio %s: %s\n", audiodev, snd_strerror(err));
	else
		if ((err = snd_pcm_set_params(PlaybackHandle, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, 1, rate, 1, 100000)) < 0)
			printf("Can't set sound play parameters: %s\n", snd_strerror(err));

	if ((err = snd_pcm_open(&RecordHandle, audiodev, SND_PCM_STREAM_CAPTURE , SND_PCM_NONBLOCK)) < 0)
		printf("Can't open audio %s: %s\n", audiodev, snd_strerror(err));
	else
		if ((err = snd_pcm_set_params(RecordHandle, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, 1, rate, 1, 100000)) < 0)
			printf("Can't set sound record parameters: %s\n", snd_strerror(err));
#ifdef USE_MIXER
	setVolume(128);
	setVolume(128);
#endif

	outBufPos = 0;
	inBufPos = 0;

	mutex = new QMutex();
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
	//timer->start(AUDIO_TIMEOUT);

}

Audio::~Audio() {
	snd_pcm_close(PlaybackHandle);
	snd_pcm_close(RecordHandle);
#ifdef USE_MIXER
	snd_mixer_close(mixerHandle);
#endif
}

void Audio::audioMute(bool m) {
	mute = m;
}

void Audio::setTX(bool m) {
	tx = m;
}

void Audio::audioRX(QByteArray out) {
	//if (tx)
	//	return;
	mutex->lock();
	if (outBufPos + out.size()/sizeof(short) >= sizeof(outBuf)/sizeof(short) ) {
		printf ("audio out overflow\n");
		outBufPos = 0;
	}
	if (mute) {
		memset(&outBuf[outBufPos], 0, out.size());
	} else {
		memcpy(&outBuf[outBufPos], out.data(), out.size());
	}
	outBufPos += out.size()/sizeof(short);
    qDebug() << "new:" << outBufPos;

    if (outBufPos < 6000) {
        if (mute) {
            memset(&outBuf[outBufPos], 0, out.size());
        } else {
            memcpy(&outBuf[outBufPos], out.data(), out.size());
        }
        outBufPos += out.size()/sizeof(short);
        qDebug() << "insert:" << outBufPos;
    }
	mutex->unlock();
}

void Audio::timeout() {
}

void Audio::run() {
	while(1) {
		mutex->lock();
//        qDebug() << "audio" << outBufPos << noutput_items;
		if (outBufPos >= noutput_items) {
			snd_pcm_sframes_t frames = snd_pcm_writei(PlaybackHandle, (char*)&outBuf[0], outBufPos);
			 // qDebug() << "f1:" << outBufPos << frames << "\r";
			// If an error, try to recover from it
			if (frames < 0)
				frames = snd_pcm_recover(PlaybackHandle, frames, 0);

			if (frames>0) {
				if (outBufPos - frames > 0) 
					memmove(outBuf, &outBuf[frames], (outBufPos - frames)*sizeof(short));
				outBufPos -= frames;
				if (noutput_items>AUDIO_BUF_SIZE/2) 
					noutput_items=AUDIO_BUF_SIZE/2;
				if (outBufPos > AUDIO_BUF_SIZE*2) {
					qDebug() << "soundcard clock to slow, skip" << AUDIO_BUF_SIZE*3/4 << "samples";
					outBufPos = AUDIO_BUF_SIZE/2;
				}
			}
		} else {
			qDebug() << "sound buffer empty, skip";
		}


		if (tx) {
			snd_pcm_sframes_t frames = sizeof(inBuf)/sizeof(short);
			// read buffer empty
			while (frames > audioRate*AUDIO_TIMEOUT/1000*2) {
				frames = snd_pcm_readi(RecordHandle, (char*)&inBuf[0], sizeof(inBuf)/sizeof(short));
				// qDebug() << frames;
				if (frames < 0) {
					frames = snd_pcm_recover(RecordHandle, frames, 0);
					qDebug() << "tx snd error" << frames;
				}
			}
			if (frames>0)
				emit audioTX(QByteArray((char*)inBuf,frames*sizeof(short)));
		}
		mutex->unlock();
		msleep(AUDIO_TIMEOUT);
	}
}

#ifdef USE_MIXER
void Audio::setVolume(int volume) {
	snd_mixer_selem_set_playback_volume_all(elemVol, volume * volumeMax / 256);
}

void Audio::setMic(int mic) {
	snd_mixer_selem_set_playback_volume_all(elemMic, mic * micMax / 256);
}
#endif
