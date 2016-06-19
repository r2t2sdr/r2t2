/**
* @file r2t2.cpp
* @brief R2T2 server application
* @author Stefan Goerg, DL2STG 
* @version 1.0
* @date 11/2015 
*/


/* Copyright (C)
* 2015 - Stefan Goerg, DL2STG
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include <vector>
#include <math.h>
#include <assert.h>

#include "r2t2.h"
#include "socket.h"
#include "reg.h"
#include "spi.h"
#include "r2t2defs.h"
#include "messages.h"
#include "client.h"
#include "receiver.h"

#define SCALE_FACTOR_24B 8388607.0         // 2^24 / 2 - 1 = 8388607.0
#define SCALE_FACTOR_32B 2147483647.0      // 2^32 / 2 - 1 = 2147483647.0
#define SCALE_FACTOR_0   0.0
#define SCALE_FACTOR_1   1.0

#define PREDIVIDER (2*16)
struct AsynchCtxData {
    void *pUserData;
    R2T2_CB udcf ;
    int run;
};

#define FFT_SKIP_FRAME_CNT	1

int user_data_callback(void *buf, int buf_size, void *extra)
{
	// The buffer received contains 32-bit signed integer IQ samples (8 bytes per sample)

	int32_t	*samplebuf 	= ((int32_t*)(buf));
	samplebuf += R2T2_HEADER_SIZE / sizeof(samplebuf[0]);
	int nSamples = (buf_size - R2T2_HEADER_SIZE)/8;      // each sample is a 24 Bit I/Q, padded to 32 Bit 
	static int cnt=0;
	static int fftcnt=0;
	static int fftRx = 0;
	RECEIVER *pRec;
	static int frameCnt = 0;

	uint8_t rxInfo = samplebuf[0] >> 24;
	int rx = rxInfo & 7;
	bool fft = (rxInfo & 0x08)>0;
	bool last = (rxInfo & 0x20)>0;
	//printf ("rx %i, fft %i, fftRx %i, last %i, cnt %i, samples %i\n",rx,fft,fftRx,last,frameCnt,nSamples);

	if (fft) {
		//printf ("rx %i, fft %i, fftRx %i, last %i, cnt %i, samples %i\n",rx,fft,fftRx,last,frameCnt,nSamples);
		pRec = &receiver[fftRx];
		if (!pRec->client || pRec->client->fft_port<0 ) {
			if (last) {
				if (frameCnt == 0) {
					fftRx++;
					fftRx &= MAX_RX-1;
					frameCnt = FFT_SKIP_FRAME_CNT;
					setMem(REG_RX_FREQ + 8 * REG_RX_OFFSET, readMem(REG_RX_FREQ + fftRx * REG_RX_OFFSET));
				} else {
					frameCnt--;
				}
			}

			return 0;
		}

		if (frameCnt == 0) {
			for (int k=0; k < nSamples; k++) {

				if ((fftcnt++ % 20480) == 0) {
					fprintf (stderr, "#");
					fflush(stderr);
				}

				// copy into the output buffer, converting to float and rescaling
				//pRec->fft_buffer[k*2]   = ((float) (samplebuf[k*2]   << 13)) / SCALE_FACTOR_32B;
				//pRec->fft_buffer[k*2]   = ((float) (samplebuf[k*2]   << 13)) / SCALE_FACTOR_32B;
				pRec->fft_buffer[k*2] = ((float) (samplebuf[k*2] << 8)) / SCALE_FACTOR_32B;
				pRec->fft_buffer[k*2+1] = ((float) (samplebuf[k*2+1] << 8)) / SCALE_FACTOR_32B;
			}
			pRec->fftSamples = nSamples;
			send_FFT_buffer(pRec, last);

			if (last) {
				for (int i=0;i<MAX_RX;i++) {
					fftRx++;
					fftRx &= MAX_RX-1;
					pRec = &receiver[fftRx];
					if (pRec->client && pRec->client->fft_port>=0) {
						frameCnt = FFT_SKIP_FRAME_CNT;
						setMem(REG_RX_FREQ + 8 * REG_RX_OFFSET, readMem(REG_RX_FREQ + fftRx * REG_RX_OFFSET));
						break;
					}
				}
			}
		} else {
			if (last)
				frameCnt--;
		}

		return 0;
	}

	pRec = &receiver[rx];

	if (!pRec->client)
		return 0;
	if (pRec->client->iq_port < 0)
		return 0;

	for (int k=0; k < nSamples; k++) {

		if ((cnt++ % 20480) == 0) {
			fprintf (stderr, ".");
			fflush(stderr);
		}

		pRec->input_buffer[pRec->samples]             = ((float) (samplebuf[k*2+1] << 10))   / SCALE_FACTOR_32B;
		pRec->input_buffer[pRec->samples+BUFFER_SIZE] = ((float) (samplebuf[k*2] << 10)) / SCALE_FACTOR_32B;

		uint8_t rxInfo1 = samplebuf[0] >> 24;
		if ((rxInfo1 & 7) != rx)
			fprintf (stderr, "ERROR\n");;

		pRec->samples++; // next output sample 

		// when we have enough samples, send them to the client
		if(pRec->samples==BUFFER_SIZE) {
			// send I/Q data to clients
			send_IQ_buffer(pRec);
			pRec->samples=0;      // signal that the output buffer is empty again
		}
	}
	return 0;
}


struct PreselItem { // Holds item data from the r2t2.config file
	long long freq;
	unsigned int filtNum;
    char *desc;
};

struct R2t2 {
    // network parameters
    int socket;

    // radio state
    long long freq[MAX_RX];
    long long bw[MAX_RX];
    int  attDb;
    int  antSel;
    unsigned int  preSel; // Holds filter number
    int  preamp;
	int clock;

    // asynch thread for receiving data from hardware
    pthread_t      thread_id;
    AsynchCtxData *pacd;
};

// module variables
static struct R2t2 r2t2;
static int init = 0;


int r2t2_init (int r2t2_bw, long long r2t2_f)
{
   if (init) { 
       fprintf (stderr, "%s: WARNING: attempting to double init !\n", __FUNCTION__);
       return -1;
   }

   initSpi();
  
   r2t2.clock = DEFAULT_SAMPLE_RATE;
   for (int i=0;i<MAX_RX;i++) {
	   r2t2.freq[i] = r2t2_f;
	   r2t2.bw[i] = r2t2_bw;
   }
   r2t2.attDb  = 0;
   r2t2.antSel = 0;
   r2t2.preSel = 0;
   r2t2.preamp = 0;

   // asynch reception thread data
   r2t2.pacd      = 0;
   r2t2.thread_id = 0;
   r2t2.socket = connectR2T2("rad0");

   for (int i=0;i<MAX_RX;i++) {
	   r2t2_set_bandwidth (i, r2t2.bw[i]);
	   r2t2_set_frequency (i, r2t2.freq[i]);
   }

   if ( r2t2_start_asynch_input ( user_data_callback, &receiver) < 0 )
	   printf("start async input error\n" );
   return 0;

}

int r2t2_set_frequency (int rx, long long f)
{
	fprintf (stderr, "%s: %Ld\n", __FUNCTION__, f);
	if (rx>=MAX_RX) {
		assert(0);
		return 0;
	}
	r2t2.freq[rx] = f;
	uint32_t phInc = pow(2,30)*f/r2t2.clock;
	setMem(REG_RX_FREQ + rx * REG_RX_OFFSET, phInc);
	
	return 0;
}

int r2t2_set_bandwidth (int rx, long long b)
{
   fprintf (stderr, "%s: %Ld\n", __FUNCTION__, b);

	if (rx>=MAX_RX) {
		assert(0);
		return 0;
	}

   r2t2.bw[rx] = b;
   uint32_t div = r2t2.clock/b/PREDIVIDER;
   printf ("clock %i divider %i,  rate %i\n",r2t2.clock, div, r2t2.clock/div/PREDIVIDER);
   setMem(REG_RX_DIVIDER + rx * REG_RX_OFFSET, div);

   return 0;
}

int r2t2_set_attenuator (int attDb)
{
   r2t2.attDb = attDb; 
   setRxAtt(attDb, 0);
   setRxAtt(attDb, 1);
   fprintf (stderr, "%s: requested value: %d computed value: %02X\n", __FUNCTION__, attDb, r2t2.attDb);
   return 0;
}

int r2t2_set_antenna_input (int n)
{
    r2t2.antSel = n == 0 ? 0x00 : 0x01;
    fprintf (stderr, "%s: antenna: %02X\n", __FUNCTION__, r2t2.antSel);
    return 0;
}

int r2t2_set_preselector (int p)
{
    r2t2.preSel = (p & 0x0F);

    fprintf (stderr, "%s: preselector unimplemented: %02X\n", __FUNCTION__, r2t2.preSel);
    return 0;
}


int r2t2_set_preamp (int newstatus)
{
	if (newstatus != 0) 
		r2t2.preamp = 20;
	else 
		r2t2.preamp = 0;

	setRxGain(r2t2.preamp, 0);
	setRxGain(r2t2.preamp, 1);
	fprintf (stderr, "%s: preamplifier: %02X\n", __FUNCTION__, r2t2.preamp);
	return 0;
}

int r2t2_get_preamp (void)
{ 
    return r2t2.preamp;
}

int r2t2_deinit (void)
{
   disconnectR2T2(r2t2.socket);
   init = 0;
   return 0;
}

static int get_data (struct R2t2 *hiq, unsigned char *buffer, int buf_len);

static void *asynch_input_thread (void *p)
{
    AsynchCtxData *pacd = (AsynchCtxData *)p;

    while (pacd->run) {
        unsigned char buffer [8192];
        int buf_len = get_data (&r2t2, buffer, sizeof(buffer));

        if (buf_len > 0) {
           int rcb = pacd->udcf ((void *)buffer, buf_len, pacd->pUserData);
           if (rcb < 0) break;  // user callback asked for termination
        } else {
            pacd->udcf ((void *)0, 0, 0);
            break;
        }
    }
    return 0;
}

int r2t2_start_asynch_input (R2T2_CB cb, void *pud)
{
    int rc;

    if (r2t2.pacd) 
		delete r2t2.pacd;

    r2t2.pacd = new AsynchCtxData;

    r2t2.pacd->pUserData = NULL;
    r2t2.pacd->udcf = cb;
    r2t2.pacd->run = 1;

    // create the thread to receive data
    rc = pthread_create(&(r2t2.thread_id),NULL, asynch_input_thread,(void *)(r2t2.pacd));
    if(rc < 0) {
        perror("pthread_create asynch_input_thread failed");
    } else {
    }
    return rc;
    
}


int r2t2_stop_asynch_input ()
{
    int rc = -1;

    if (r2t2.thread_id != 0) {
        void *pExit;

        r2t2.pacd->run = 0;
        rc = pthread_join (r2t2.thread_id, &pExit);
    }

    return rc;
}

// get audio from a client
static int get_data (struct R2t2 *hiq, unsigned char *buffer, int buf_len)
{
	return readSocket(r2t2.socket, buffer, buf_len);
}

