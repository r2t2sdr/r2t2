/**
* @file receiver.h
* @brief manage client attachment to receivers
* @author John Melton, G0ORX/N6LYT
* @version 0.1
* @date 2009-10-13
*/

/* Copyright (C)
* 2009 - John Melton, G0ORX/N6LYT
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
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


#define BUFFER_SIZE 	1024
#define FFT_BUFFER_SIZE 1024

struct R2T2Config {
    std::string  ip;
    int   sr;
};



typedef struct _receiver {
    int id;
    int audio_socket;
    pthread_t audio_thread_id;
    CLIENT* client;
    int frequency_changed;
    long frequency;
    float fft_buffer[FFT_BUFFER_SIZE*2];
	int fftSamples;
    float input_buffer[BUFFER_SIZE*2];
    float output_buffer[BUFFER_SIZE*2];
    int samples;
    //
    // specific to R2T2 
    //
    R2T2Config cfg;
    // Average DC component in samples  
    //
    float dc_average_i;
    float dc_average_q;
    float dc_sum_i;
    float dc_sum_q;
    int   dc_count;
    int   dc_key_delay;
	int offset;

} RECEIVER;

extern RECEIVER receiver[MAX_RX];

typedef struct _buffer {
    unsigned long long sequence;
    unsigned short offset;
    unsigned short length;
    unsigned char data[500];
} BUFFER;

void init_receivers(R2T2Config *);
const char* attach_receiver(int rx,CLIENT* client);
const char* detach_receiver(int rx,CLIENT* client);
const char* set_frequency(CLIENT* client,long f);
const char* set_dither(CLIENT* client, bool);
const char* set_random(CLIENT* client, bool);
const char* set_attenuator(CLIENT* client, int);
const char* select_antenna (CLIENT* client, int antenna);
const char* select_preselector (CLIENT* client, int preselector);
const char* set_preamplifier(CLIENT* client, int);
void send_IQ_buffer (RECEIVER *pRec);
void send_FFT_buffer (RECEIVER *pRec, bool last);

