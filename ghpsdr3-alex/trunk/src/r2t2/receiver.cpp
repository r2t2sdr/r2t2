/**
* @file receiver.c
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <string>

#include "messages.h"
#include "client.h"
#include "r2t2defs.h"
#include "receiver.h"
#include "r2t2.h"

#define  SMALL_PACKETS

RECEIVER receiver[MAX_RX];
static int iq_socket;
static struct sockaddr_in iq_addr;
static int iq_length;

static char response[80];

static unsigned long sequence=0L;
static unsigned long fft_sequence=0L;

static int CORE_BANDWIDTH;

void init_receivers (R2T2Config *pCfg) 
{
    int i;
    for(i=0;i<MAX_RX;i++) {
        receiver[i].client  = (CLIENT*)NULL;
        receiver[i].samples = 0; 
        receiver[i].fftSamples = 0; 
    }

    iq_socket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(iq_socket<0) {
        perror("create socket failed for iq_socket\n");
        exit(1);
    }

    iq_length=sizeof(iq_addr);
    memset(&iq_addr,0,iq_length);
    iq_addr.sin_family=AF_INET;
    iq_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    iq_addr.sin_port=htons(11002);

    if(bind(iq_socket,(struct sockaddr*)&iq_addr,iq_length)<0) {
        perror("bind socket failed for iq socket");
        exit(1);
    }

    CORE_BANDWIDTH = pCfg->sr;

}

const char* attach_receiver(int rx, CLIENT* client) 
{
    if(client->state==RECEIVER_ATTACHED) {
        return CLIENT_ATTACHED;
    }

	if (rx >= MAX_RX)
		return RECEIVER_INVALID;

    r2t2_set_frequency (rx, 7050000LL);
    r2t2_set_bandwidth (rx, CORE_BANDWIDTH);

    if(receiver[rx].client!=(CLIENT *)NULL) {
        return RECEIVER_IN_USE;
    }
    
    client->state=RECEIVER_ATTACHED;
    receiver[rx].client=client;
    client->receiver=rx;

    receiver[rx].dc_average_i  = 0 ;
    receiver[rx].dc_average_q  = 0 ; 
    receiver[rx].dc_sum_i      = 0 ; 
    receiver[rx].dc_sum_q      = 0 ; 
    receiver[rx].dc_count      = 0 ; 
    receiver[rx].dc_key_delay  = 0 ; 
    receiver[rx].offset        = 0 ; 

    sprintf(response,"%s %d",OK, CORE_BANDWIDTH);

    return response;
}

const char* detach_receiver (int rx, CLIENT* client) {
    if(client->state==RECEIVER_DETACHED) {
        return CLIENT_DETACHED;
    }

    if(rx >= MAX_RX)
        return RECEIVER_INVALID;

    if(receiver[rx].client!=client)
        return RECEIVER_NOT_OWNER;

    printf("detach_receiver: ...");

    r2t2_stop_asynch_input ();
    printf(" done.\n");

    client->state=RECEIVER_DETACHED;
    receiver[rx].client = (CLIENT*)NULL;

    return OK;
}

const char* set_frequency (CLIENT* client, long frequency) {
    if(client->state==RECEIVER_DETACHED) {
        return CLIENT_DETACHED;
    }

    if(client->receiver<0) {
        return RECEIVER_INVALID;
    }

    receiver[client->receiver].frequency=frequency;
    receiver[client->receiver].frequency_changed=1;

    //fprintf (stderr, "%s: %ld\n", __FUNCTION__, receiver[client->receiver].frequency);

    r2t2_set_frequency (client->receiver, frequency);

    return OK;
}

const char* set_dither (CLIENT* client, bool dither)
{
//  receiver[client->receiver].ppc->dither = dither;
//
    return NOT_IMPLEMENTED_COMMAND;
    return OK;
}

const char* set_random (CLIENT* client, bool)
{
    return NOT_IMPLEMENTED_COMMAND;
    return OK;
}

const char* set_attenuator (CLIENT* client, int new_level_in_db)
{
	switch (new_level_in_db) {
		case 0:
		case 10:
		case 20:
		case 30:
		case 40:
			fprintf (stderr, "%s: new attenuator level: %d\n", __FUNCTION__, -(new_level_in_db));
			r2t2_set_attenuator (new_level_in_db);
			break;
		default:
			return INVALID_COMMAND;
	}
	return OK;
}

const char* select_antenna (CLIENT* client, int antenna)
{
    if (antenna == 0 || antenna == 1) {
        fprintf (stderr, "%s: new antenna: %d\n", __FUNCTION__, antenna);
        r2t2_set_antenna_input (antenna);
        return OK;
    } else
        return INVALID_COMMAND;
}


const char* select_preselector (CLIENT* client, int preselector)
{
	return INVALID_COMMAND;
}

const char* set_preamplifier(CLIENT* client, int preamp)
{
	fprintf (stderr, "%s: new prepreamp: %d\n", __FUNCTION__, preamp);
	r2t2_set_preamp (preamp);
	return OK;
}

void send_IQ_buffer (RECEIVER *pRec) {
    struct sockaddr_in client;
    int client_length;
    static unsigned short offset=0;
    BUFFER buffer;
    int rc;
	int port = pRec->client->iq_port;


    if(pRec->client != (CLIENT*)NULL) {
        if(port != -1) {
            // send the IQ buffer

            client_length = sizeof(client);
            memset((char*)&client,0,client_length);
            client.sin_family = AF_INET;
            client.sin_addr.s_addr = pRec->client->address.sin_addr.s_addr;
            client.sin_port = htons(port);

            // keep UDP packets to 512 bytes or less
            //     8 bytes sequency number
            //     2 byte offset
            //     2 byte length
			while(offset<sizeof(pRec->input_buffer)) {
				buffer.sequence=sequence;
				buffer.offset=offset;
				buffer.length=sizeof(pRec->input_buffer)-offset;
				if(buffer.length>500) 
					buffer.length=500;
				memcpy ((char*)&buffer.data[0], (char*)&(pRec->input_buffer[offset/4]), buffer.length);
				rc = sendto (iq_socket, (char*)&buffer, sizeof(buffer), 0, (struct sockaddr*)&client,client_length);
				if(rc<=0) {
					perror("sendto failed for iq data");
					exit(1);
				} 
				//else {
				//    fprintf (stderr, "%s: sending packet to %s.\n", __FUNCTION__, inet_ntoa(client.sin_addr));
				//}
				offset+=buffer.length;
			}
			offset = 0;
			sequence++;
        }
    }
}


void send_FFT_buffer (RECEIVER *pRec, bool last) {
    struct sockaddr_in client;
    int client_length;
    BUFFER buffer;
    int rc;
	int port = pRec->client->fft_port;
	unsigned int processed = 0;

    if(pRec->client != (CLIENT*)NULL) {
        if(port != -1) {
            // send the FFT buffer

            client_length = sizeof(client);
            memset((char*)&client,0,client_length);
            client.sin_family = AF_INET;
            client.sin_addr.s_addr = pRec->client->address.sin_addr.s_addr;
            client.sin_port = htons(port);

            // keep UDP packets to 512 bytes or less
            //     8 bytes sequency number
            //     2 byte offset
            //     2 byte length
			while(processed < pRec->fftSamples * sizeof(float)*2) {
				buffer.sequence = fft_sequence++;
				buffer.offset = pRec->offset;
				buffer.length = (pRec->fftSamples * sizeof(float)*2) - processed;
				if(buffer.length>500) 
					buffer.length=500;
				memcpy ((char*)&buffer.data[0], (char*)&(pRec->fft_buffer[processed/4]), buffer.length);
				rc = sendto (iq_socket, (char*)&buffer, sizeof(buffer), 0, (struct sockaddr*)&client,client_length);
				if(rc<=0) {
					perror("sendto failed for fft data");
					exit(1);
				} 
				pRec->offset += buffer.length;
				processed += buffer.length;
			}
			if (last)
				pRec->offset = 0;
        }
    }
}
