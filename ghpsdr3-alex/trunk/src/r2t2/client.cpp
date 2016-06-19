/**
 * @file client.c
 * @brief Handle client connection
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <string>     // c++ std strings

#include "r2t2.h"
#include "r2t2defs.h"
#include "client.h"
#include "receiver.h"
#include "messages.h"
#include "socket.h"



#define ADC_CLIP 0x01



const char* parse_command(CLIENT* client,char* command);

void* client_thread(void* arg) {
	CLIENT* client=(CLIENT*)arg;
	char command[80];
	int bytes_read;
	const char* response;

	fprintf(stderr,"%s: client connected: %s:%d\n", __FUNCTION__, inet_ntoa(client->address.sin_addr),ntohs(client->address.sin_port));

	client->state=RECEIVER_DETACHED;

	while(1) {
		bytes_read=recv(client->socket,command,sizeof(command),0);
		if(bytes_read<=0) {
			break;
		}
		command[bytes_read]=0;
		response=parse_command(client,command);
		send(client->socket,response,strlen(response),0);

		//fprintf(stderr,"%s: response: '%s'\n", __FUNCTION__, response);

		if (strcmp(response, QUIT_ASAP) == 0) {
			break;
		}
	}

	close(client->socket);

	fprintf(stderr,"%s: client disconnected: %s:%d\n", __FUNCTION__, inet_ntoa(client->address.sin_addr),ntohs(client->address.sin_port));

	if(client->state==RECEIVER_ATTACHED) {
		//int rx = client->receiver;
		//free(client);
		//receiver[rx].client=(CLIENT*)NULL;
		//client->state=RECEIVER_DETACHED;

		detach_receiver (client->receiver, client);

	}
	return 0;
}

const char* parse_command(CLIENT* client,char* command) {

	char* token;

	fprintf(stderr,"parse_command: '%s'\n",command);

	token=strtok(command," \r\n");
	if(token!=NULL) {
		if(strcmp(token,"attach")==0) {
			// select receiver
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				int rx=atoi(token);
				return attach_receiver(rx,client);
			} else {
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"detach")==0) {
			// select receiver
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				int rx=atoi(token);
				return detach_receiver(rx,client);
			} else {
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"frequency")==0) {
			// set frequency
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				long f=atol(token);
				return set_frequency (client,f);
			} else {
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"start")==0) {
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				if(strcmp (token,"iq")==0) {
					token=strtok(NULL," \r\n");
					if(token!=NULL) {
						client->iq_port=atoi(token);
					}
					printf("*************************** CLIENT REQUESTED IQ  %d port\n", client->iq_port);
					return OK;

				} else if(strcmp(token,"fft")==0) {
					token=strtok(NULL," \r\n");
					if(token!=NULL) {
						client->fft_port=atoi(token);
					}
					printf("*************************** CLIENT REQUESTED FFT %d port\n", client->fft_port);
					return OK;
				} else if(strcmp(token,"bandscope")==0) {
					token=strtok(NULL," \r\n");
					if(token!=NULL) {
						client->bs_port=atoi(token);
					}
					return OK;

				} else {
					// invalid command string
					return INVALID_COMMAND;
				}
			} else {
				// invalid command string
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"dither")==0) {
			// set frequency
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				if (strcmp(token,"on")==0) {
					return set_dither (client,true);
				}
				if (strcmp(token,"off")==0) {
					return set_dither (client,false);
				}
				return INVALID_COMMAND;
			} else {
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"setattenuator")==0) {
			// set attenuator
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				long av=atol(token);
				return set_attenuator (client,av);
			} else {
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"selectantenna")==0) {
			// select antenna
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				long antenna = atol(token);
				return select_antenna (client,antenna);
			} else {
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"selectpresel")==0) {
			// select preselector
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				long presel = atol(token);
				return select_preselector (client,presel);
			} else {
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"activatepreamp")==0) {
			// activate preamplifier
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				long preamp = atol(token);
				return set_preamplifier (client,preamp);
			} else {
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"random")==0) {
			// set frequency
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				if (strcmp(token,"on")==0) {
					return set_random (client,true);
				}
				if (strcmp(token,"off")==0) {
					return set_random (client,false);
				}
				return INVALID_COMMAND;
			} else {
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"stop")==0) {
			token=strtok(NULL," \r\n");
			if(token!=NULL) {
				if(strcmp(token,"iq")==0) {
					token=strtok(NULL," \r\n");
					if(token!=NULL) {
						client->iq_port=-1;
					}
					// try to terminate audio thread
					close ((receiver[client->receiver]).audio_socket);
					printf("Quitting...\n");
					r2t2_stop_asynch_input ();
					return OK;
				} else if(strcmp(token,"fft")==0) {
					client->fft_port=-1;
				} else if(strcmp(token,"bandscope")==0) {
					client->bs_port=-1;
				} else {
					// invalid command string
					return INVALID_COMMAND;
				}
			} else {
				// invalid command string
				return INVALID_COMMAND;
			}
		} else if(strcmp(token,"quit")==0) {
			return QUIT_ASAP;

		} else if(strcmp(token,"hardware?")==0) {
			return "OK HiQSDR";

		} else if(strcmp(token,"getserial?")==0) {
			// no serial number concept available in R2T2
			static char buf[50];
			// TODO serial number ?
			snprintf (buf, sizeof(buf), "OK R2T2");
			return buf;

		} else if(strcmp(token,"getpreselector?")==0) {
			// get preselector
			return INVALID_COMMAND;

		} else if(strcmp(token,"getpreampstatus?")==0) {
			// returns preamp status
			static char buf[50];
			snprintf (buf, sizeof(buf), "OK %d", r2t2_get_preamp ());
			return buf;

		} else {
			// invalid command string
			return INVALID_COMMAND;
		}
	} else {
		// empty command string
		return INVALID_COMMAND;
	}
	return INVALID_COMMAND;
}

