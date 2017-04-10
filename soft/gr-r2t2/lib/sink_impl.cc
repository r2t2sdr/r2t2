/* -*- c++ -*- */
/* 
 * Copyright 2013 Stefan Goerg stefan@dl2stg.de
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "sink_impl.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


// #include <gruel/thread.h>

namespace gr {
	namespace r2t2 {

		sink::sptr sink::make(size_t itemsize, const char *host, unsigned short port, int freq, int rate) {
			return gnuradio::get_initial_sptr (new sink_impl(itemsize, host, port, freq, rate));
		}

		/*
		 * The private constructor
		 */
		sink_impl::sink_impl(size_t itemsize, const char *host, unsigned short port, int freq, int rate) 
			: gr::sync_block("sink", gr::io_signature::make(1, 1, itemsize), gr::io_signature::make(0, 0, 0)) {

				r2t2Socket = openSocket(host, port);
				r2t2ServerMsg = new R2T2Proto::R2T2Message();
				r2t2Msg = new R2T2Proto::R2T2Message();

				r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_TXOPEN);
				sendR2T2Msg(); 

				setTxFreq(freq);
				setRate(rate);
			}

		/*
		 * Our virtual destructor.
		 */
		sink_impl::~sink_impl() {

			mutex.lock();
			r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_CLOSE);
			sendR2T2Msg(); 
			mutex.unlock();
            sleep(1);

			if (r2t2Socket != 0){
				shutdown(r2t2Socket, SHUT_RDWR);
				::close(r2t2Socket);
			}
			delete r2t2Msg;
			delete r2t2ServerMsg;
		}

		int sink_impl::work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) {
			const char *in = (const char *) input_items[0];
			int i,items = 0;

			while (1) {
				ssize_t r=0;
				fd_set readfds;
				timeval timeout;
				timeout.tv_sec = 0;	  // Init timeout each iteration.  Select can modify it.
				timeout.tv_usec = 100;
				FD_ZERO(&readfds);
				FD_SET(r2t2Socket, &readfds);
				r = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
				if(r < 0) {
					perror("r2t2: select");
					break;
				} else if(r > 0 ) {  // data available 

					// This is a non-blocking call with a timeout set in the constructor
					r = recv(r2t2Socket, inBuf, sizeof(inBuf), 0);  // get the entire payload or the what's available
					// printf ("rcv %i\n",r);
					// Check if there was a problem; forget it if the operation just timed out
					if(r < 0) {
						if( errno !=  EAGAIN) {  // handle non-blocking call timeout
							perror("r2t2 recv");
						}
						break;
					} else {
						// handle incomming messages
						int pos = 0;

						if (inBuf[0]!='R' || inBuf[1]!='2') {
							printf("sync error: magic");
							return -1;
						} else {

							int len = r;
							while(len > 0) {

								int pktLen = inBuf[pos+2]+(inBuf[pos+3]<<8);
								if (pktLen > len) {
									printf("sync error: len");
									break; 
								}

								if (r2t2Msg->ParseFromArray(&inBuf[pos+4], pktLen)) {

									if (r2t2Msg->has_tx()) {
										connected = r2t2Msg->tx()+1;	
										dataReq = 20;// prefill messages
                                        lastAck = 0; 
										printf ("connected to tx %i\n",r2t2Msg->tx());
									}

                                    if (r2t2Msg->has_txdataack()) {
                                        uint32_t ackSeq = r2t2Msg->txdataack(); 
                                        if ((ackSeq - lastAck) > 1)
                                            printf ("ack lost %i %i\n",ackSeq, ackSeq-lastAck);
                                        dataReq += ackSeq - lastAck;
                                        lastAck = ackSeq; 
                                        //printf("ack seq %i %i\n", dataReq, lastAck);
                                    }

									if (r2t2Msg->has_command()) {
										switch(r2t2Msg->command()) {
											case R2T2Proto::R2T2Message_Command_ACK:
												break;
											case R2T2Proto::R2T2Message_Command_NACK:
												break;
											case R2T2Proto::R2T2Message_Command_TIMEOUT:
												dataReq = 0;
												connected = 0;	
												break;
											default:
												assert(0);
										}

									}
								}
								len -= pktLen+4;
								pos += pktLen+4;
							}
						}
					}
				} else  // no data
					break;
			}
			// handle tx data
			int pos=0;
			int nOut = noutput_items;
			int len=0;
			//while (nOut>0 && dataReq>0) {
			if (nOut>0 && dataReq>0) {
				len = std::min(180, nOut); 
				mutex.lock();
				r2t2ServerMsg->set_txdata(in+pos, len*sizeof(gr_complex));
				mutex.unlock();
				// printf("txData %i %i %i\n", nOut, len, dataReq);
				dataReq--;
				sendR2T2Msg();
				nOut-=len;
				pos += len*sizeof(gr_complex);
			}
			return pos/sizeof(gr_complex);
		}

		void sink_impl::setTxFreq(int f) {
			printf ("set freq %i\n",f);
			mutex.lock();
			r2t2ServerMsg->set_txfreq(f);
			sendR2T2Msg(); 
			mutex.unlock();
		}

		void sink_impl::setRate(int rate) {
			printf ("set rate %i\n",rate);
			mutex.lock();
			r2t2ServerMsg->set_txrate(rate);
			sendR2T2Msg(); 
			mutex.unlock();
		}


		void sink_impl::sendR2T2Msg() {

			if (r2t2ServerMsg->ByteSize() == 0)
				return;

			std::ostringstream out;
			r2t2ServerMsg->SerializeToOstream(&out);

			strcpy((char*)outBuf, "R2");
			outBuf[2] = out.str().size() & 0xff;
			outBuf[3] = (out.str().size() >> 8) & 0xff;

			memcpy(outBuf+4, out.str().data(), out.str().size());
			//printf("out %i\n",out.str().size()+4);
			send(r2t2Socket, outBuf, out.str().size()+4, 0);

			r2t2ServerMsg->Clear();
		}


		int sink_impl::openSocket (const char *host, unsigned short port) {

			int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if(sock == -1) {
				perror("r2t2: socket open");
				return 0;
			}

			// Don't wait when shutting down
			linger lngr;
			lngr.l_onoff  = 1;
			lngr.l_linger = 0;
			if(setsockopt(sock, SOL_SOCKET, SO_LINGER, &lngr, sizeof(linger)) == -1) {
				if( errno != ENOPROTOOPT) {  // no SO_LINGER for SOCK_DGRAM on Windows
					perror("r2t2: setsockopt"); 
					return 0;
				}
			}

			if(host != NULL ) {
				// Get the destination address
				struct addrinfo *ip_dst;
				struct addrinfo hints;
				memset( (void*)&hints, 0, sizeof(hints) );
				hints.ai_family = AF_INET;
				hints.ai_socktype = SOCK_DGRAM;
				hints.ai_protocol = IPPROTO_UDP;
				char port_str[12];
				sprintf( port_str, "%d", port );

				if (getaddrinfo( host, port_str, &hints, &ip_dst )!=0) {
					perror("r2t2: socket getaddrinfo");
					return 0;
				}

				if(::connect(sock, ip_dst->ai_addr, ip_dst->ai_addrlen) == -1) {
					perror("r2t2: socket connect");
					return 0;
				}
				freeaddrinfo(ip_dst);
			}
			return sock;
		}

		// Return port number of r2t2Socket
		int sink_impl::get_port(void)
		{
			sockaddr_in name;
			socklen_t len = sizeof(name);
			int ret = getsockname( r2t2Socket, (sockaddr*)&name, &len );
			if( ret ) {
				perror("sink_impl getsockname");
				return -1;
			}
			return ntohs(name.sin_port);
		}

		void sink_impl::closeSocket(int socket) {
			timeval timeout;
			timeout.tv_sec = 0;    // zero time for immediate return
			timeout.tv_usec = 0;
			fd_set readfds;
			FD_ZERO(&readfds);
			FD_SET(socket, &readfds);
			int r = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
			if(r < 0) {
				perror("r2t2 select");
			}
			else if(r > 0) {  // call recv() to get error return
				r = recv(socket, (char*)&readfds, sizeof(readfds), 0);
				if(r < 0) {
					perror("r2t2 recv");
				}
			}
		}

		void sink_impl::disconnect()
		{
			closeSocket(r2t2Socket);
			return;
		}

	} /* namespace r2t2 */
} /* namespace gr */

