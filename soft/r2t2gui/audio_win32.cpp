#include <QTime>
#include <QDebug>
#include <stdio.h>
#include <assert.h>
#include "config.h"
#include "audio_win32.h"
#include "lib.h"

#define AUDIO_TIMEOUT	20	

Audio::Audio(char* dev, char* mixerDev, char* mixerVol, char* mixerMic, int rate)  {

	int err;
	tx = false;
	mute = false;
	lastOutPos = 0;
	noutput_items = 1024;

	pWaveHdr1  = reinterpret_cast <PWAVEHDR> ( malloc (sizeof (WAVEHDR)) );
	pWaveHdr2  = reinterpret_cast <PWAVEHDR> ( malloc (sizeof (WAVEHDR)) ) ;
	pWaveHdrRx = reinterpret_cast <PWAVEHDR> ( malloc (sizeof (WAVEHDR)) ) ;

	// Open waveform audio for input
	WAVEFORMATEX waveform ;

	waveform.wFormatTag      = WAVE_FORMAT_PCM;
	waveform.nChannels       = 1 ;
	waveform.nSamplesPerSec  = rate;
	waveform.nAvgBytesPerSec = rate*2;
	waveform.wBitsPerSample  = 16 ;
	waveform.nBlockAlign     = waveform.wBitsPerSample/(waveform.nChannels*8) ;
	waveform.cbSize          = 0 ;

	if (int err = waveInOpen (&hWaveIn, WAVE_MAPPER, &waveform, (DWORD) NULL /*hwnd*/, 0, CALLBACK_NULL /* CALLBACK_WINDOW*/)) {
		printf ("err %i\n",err);
		assert(0);
	}

	// Open waveform audio for output 
	waveform.wFormatTag      = WAVE_FORMAT_PCM;
	waveform.nChannels       = 1 ;
	waveform.nSamplesPerSec  = rate;
	waveform.nAvgBytesPerSec = rate*2;
	waveform.wBitsPerSample  = 16;
	waveform.nBlockAlign     = waveform.wBitsPerSample/(waveform.nChannels*8) ;
	waveform.cbSize          = 0 ;

	if (err = waveOutOpen (&hWaveOut, WAVE_MAPPER, &waveform, (DWORD) NULL /*hwnd*/, 0, CALLBACK_NULL /*CALLBACK_WINDOW*/)) {
		printf ("err %i\n",err);
		assert(0);
	}

	pBuffer1=reinterpret_cast <PBYTE> (malloc(AUDIO_IN_BUFSIZE*2) );
	pBuffer2= reinterpret_cast <PBYTE> ( malloc(AUDIO_IN_BUFSIZE*2) );

	outBufPos = 0;

	mutex = new QMutex();
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
	//timer->start(AUDIO_TIMEOUT);

}

Audio::~Audio() {
	waveInStop(hWaveIn);
	waveInUnprepareHeader (hWaveIn, pWaveHdr1, sizeof (WAVEHDR)) ;
	waveInUnprepareHeader (hWaveIn, pWaveHdr2, sizeof (WAVEHDR)) ;
	waveOutUnprepareHeader (hWaveOut, pWaveHdrRx, sizeof (WAVEHDR)) ;
	waveInClose (hWaveIn) ;
	waveOutClose (hWaveOut) ;
	free(pBuffer1);
	free(pBuffer2);
}

void Audio::audioMute(bool m) {
	mute = m;
}

void Audio::setTX(bool m) {
	tx = m;
	if (tx) {
		// Set up headers and prepare them

		pWaveHdr1->lpData          =reinterpret_cast <CHAR*>( pBuffer1 ) ;
		pWaveHdr1->dwBufferLength  = AUDIO_IN_BUFSIZE;
		pWaveHdr1->dwBytesRecorded = 0 ;
		pWaveHdr1->dwUser          = 0 ;
		pWaveHdr1->dwFlags         = 0 ;
		pWaveHdr1->dwLoops         = 0 ;
		pWaveHdr1->lpNext          = NULL ;
		pWaveHdr1->reserved        = 0 ;
		waveInPrepareHeader (hWaveIn, pWaveHdr1, sizeof (WAVEHDR)) ;

		pWaveHdr2->lpData          = reinterpret_cast <CHAR*>(pBuffer2 ) ;
		pWaveHdr2->dwBufferLength  = AUDIO_IN_BUFSIZE;
		pWaveHdr2->dwBytesRecorded = 0 ;
		pWaveHdr2->dwUser          = 0 ;
		pWaveHdr2->dwFlags         = 0 ;
		pWaveHdr2->dwLoops         = 0 ;
		pWaveHdr2->lpNext          = NULL ;
		pWaveHdr2->reserved        = 0 ;
		waveInPrepareHeader (hWaveIn, pWaveHdr2, sizeof (WAVEHDR)) ;

		waveInAddBuffer (hWaveIn, pWaveHdr1, sizeof (WAVEHDR)) ;
		waveInAddBuffer (hWaveIn, pWaveHdr2, sizeof (WAVEHDR)) ;
		waveInStart (hWaveIn) ;
	} 
}

void Audio::audioRX(QByteArray out) {
	//if (tx)
	//	return;
	mutex->lock();
	if (outBufPos + out.size()/sizeof(short) >= sizeof(outBuf)/sizeof(short) ) {
		//printf ("audio out overflow\n");
		lastOutPos = 0;
		outBufPos = 0;
	}
	//printf ("in %i %i %i\n",outBufPos,lastOutPos, noutput_items);
	if (mute) {
		memset(&outBuf[outBufPos], 0, out.size());
	} else {
		memcpy(&outBuf[outBufPos], out.data(), out.size());
	}
	outBufPos += out.size()/sizeof(short);
	mutex->unlock();
}

void Audio::timeout() {
}

void Audio::run() {
	while(1) {
		mutex->lock();
		if (outBufPos-lastOutPos >= noutput_items) {
			//printf ("%i %i %i\n",outBufPos,lastOutPos, noutput_items);
			// Set up header
			pWaveHdrRx->lpData          = reinterpret_cast <CHAR*>(&outBuf[lastOutPos]) ;
			pWaveHdrRx->dwBufferLength  = (DWORD) (outBufPos-lastOutPos)*2;
			pWaveHdrRx->dwBytesRecorded = 0 ;
			pWaveHdrRx->dwUser          = 0 ;
			pWaveHdrRx->dwFlags         = 0; // WHDR_BEGINLOOP | WHDR_ENDLOOP ;
			pWaveHdrRx->dwLoops         = 1;
			pWaveHdrRx->lpNext          = NULL ;
			pWaveHdrRx->reserved        = 0 ;
			if (lastOutPos == 0) {
				lastOutPos = outBufPos;
			} else {
				lastOutPos = 0;
				outBufPos = 0;
			}
			
			// Prepare and write
			waveOutPrepareHeader (hWaveOut, pWaveHdrRx, sizeof (WAVEHDR)) ;
			waveOutWrite (hWaveOut, pWaveHdrRx, sizeof (WAVEHDR)) ;
		}

		if (tx) {
			if (pWaveHdr1->dwFlags & WHDR_DONE) {
				//printf ("tx buf1 %i\n",pWaveHdr1->dwBytesRecorded);
				emit audioTX(QByteArray((char*)pBuffer1, pWaveHdr1->dwBytesRecorded));
				pWaveHdr1->lpData          =reinterpret_cast <CHAR*>( pBuffer1 ) ;
				pWaveHdr1->dwBufferLength  = AUDIO_IN_BUFSIZE ;
				pWaveHdr1->dwBytesRecorded = 0 ;
				pWaveHdr1->dwUser          = 0 ;
				pWaveHdr1->dwFlags         = 0 ;
				pWaveHdr1->dwLoops         = 0 ;
				pWaveHdr1->lpNext          = NULL ;
				pWaveHdr1->reserved        = 0 ;
				waveInPrepareHeader (hWaveIn, pWaveHdr1, sizeof (WAVEHDR)) ;
				waveInAddBuffer (hWaveIn, pWaveHdr1, sizeof (WAVEHDR)) ;
			}
			if (pWaveHdr2->dwFlags & WHDR_DONE) {
				//printf ("tx buf2 %i\n",pWaveHdr2->dwBytesRecorded);
				emit audioTX(QByteArray((char*)pBuffer2, pWaveHdr2->dwBytesRecorded));
				pWaveHdr2->lpData          = reinterpret_cast <CHAR*>(pBuffer2 ) ;
				pWaveHdr2->dwBufferLength  = AUDIO_IN_BUFSIZE ;
				pWaveHdr2->dwBytesRecorded = 0 ;
				pWaveHdr2->dwUser          = 0 ;
				pWaveHdr2->dwFlags         = 0 ;
				pWaveHdr2->dwLoops         = 0 ;
				pWaveHdr2->lpNext          = NULL ;
				pWaveHdr2->reserved        = 0 ;
				waveInPrepareHeader (hWaveIn, pWaveHdr2, sizeof (WAVEHDR)) ;
				waveInAddBuffer (hWaveIn, pWaveHdr2, sizeof (WAVEHDR)) ;
			}
		}
		mutex->unlock();
		msleep(AUDIO_TIMEOUT);
	}
}

//#ifdef USE_MIXER
void Audio::setVolume(int volume) {
}

void Audio::setMic(int mic) {
}

void Audio::terminate() {
}
//#endif
