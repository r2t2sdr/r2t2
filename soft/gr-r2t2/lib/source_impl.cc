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
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <complex>

#include "source_impl.h"
#include "r2t2.pb.h"

namespace gr {
    namespace r2t2 {

        source::sptr
            source::make(size_t itemsize, const char* host, unsigned short port, int rxfreq, int rate, int att, int gain, int antenna)
            {
                return gnuradio::get_initial_sptr(new source_impl(itemsize, host, port, rxfreq, rate, att, gain, antenna));
            }

        /*
         * The private constructor
         */
        source_impl::source_impl(size_t itemsize, const char *host, unsigned short port, int rxfreq, int rate, int att, int gain, int antenna)
            : gr::sync_block("r2t2 source",
                    gr::io_signature::make(0, 0, 0),
                    gr::io_signature::make(1, 1, itemsize))
        {
            r2t2Socket = openSocket(host, port);
            r2t2ServerMsg = new R2T2Proto::R2T2Message();
            r2t2Msg = new R2T2Proto::R2T2Message();

			r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_RXOPEN);
            sendR2T2Msg(); 
			r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_STARTAUDIO);
            sendR2T2Msg(); 

            setRxFreq(rxfreq);
            setRate(rate);
            setAtt(att);
            setGain(gain);
            setAntenna(antenna);
        }

        /*
         * Our virtual destructor.
         */
        source_impl::~source_impl() {
            mutex.lock();
			r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_CLOSE);
            sendR2T2Msg(); 
            mutex.unlock();

            if (r2t2Socket != 0){
                shutdown(r2t2Socket, SHUT_RDWR);
                ::close(r2t2Socket);
            }
            delete r2t2Msg;
            delete r2t2ServerMsg;
        }

        int source_impl::work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) {
            gr_complex *out = (gr_complex *) output_items[0];
            ssize_t r=0;
            int i;
            int ret=0;
            static int cnt = 0;

            while (1) {
                fd_set readfds;
                timeval timeout;
                timeout.tv_sec = 1;	  // Init timeout each iteration.  Select can modify it.
                timeout.tv_usec = 0;
                FD_ZERO(&readfds);
                FD_SET(r2t2Socket, &readfds);
                r = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
                if(r < 0) {
                    perror("r2t2: select");
                    return -1;
                } else if(r == 0 ) {  // timed out
                    return -1;
                }

                // This is a non-blocking call with a timeout set in the constructor
                r = recv(r2t2Socket, inBuf, sizeof(inBuf), 0);  // get the entire payload or the what's available

                // Check if there was a problem; forget it if the operation just timed out
                if(r < 0) {
                    if( errno ==  EAGAIN) {  // handle non-blocking call timeout
                        return -1;
                    } else {
                        perror("r2t2 recv");
                        return -1;
                    }
                } else {
                    int pos = 0;

                    if (inBuf[0]!='R' || inBuf[1]!='2') {
                        printf("sync error: magic");
                        return -1;
                    }

                    int len = r;
                    while(len > 0) {

                        int pktLen = inBuf[pos+2]+(inBuf[pos+3]<<8);
                        if (pktLen > len) {
                            printf("sync error: len");
                            return -1;
                        }

                        if (r2t2Msg->ParseFromArray(&inBuf[pos+4], pktLen)) {

                            if (r2t2Msg->has_rx()) {
                                connected = r2t2Msg->rx()+1;	
                                printf ("connected to rx %i",r2t2Msg->rx());
                            }

                            if (r2t2Msg->has_rxdata()) {
                                printf ("outBufPos %i %i\n",outBufPos, noutput_items);
                                if (outBufPos + r2t2Msg->rxdata().size() > sizeof(outBuf)) {
                                    outBufPos = 0;
                                    printf ("rx buffer overflow\n");
                                }

                                memcpy(outBuf+outBufPos, r2t2Msg->rxdata().data(), r2t2Msg->rxdata().size()); 
                                outBufPos+=r2t2Msg->rxdata().size();

                                if (outBufPos >= noutput_items*sizeof(gr_complex)) {
                                    memcpy(out, outBuf, noutput_items*sizeof(gr_complex));
                                    if (outBufPos - noutput_items*sizeof(gr_complex) > 0) 
                                        memmove(outBuf, &outBuf[noutput_items*sizeof(gr_complex)], outBufPos - noutput_items*sizeof(gr_complex));
                                    outBufPos -= noutput_items*sizeof(gr_complex);
                                    ret = noutput_items;
                                } 
                            }

                            if (r2t2Msg->has_command()) {
                                switch(r2t2Msg->command()) {
                                    case R2T2Proto::R2T2Message_Command_ACK:
                                        break;
                                    case R2T2Proto::R2T2Message_Command_NACK:
                                        break;
                                    case R2T2Proto::R2T2Message_Command_TIMEOUT:
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
                    if (ret == 0)
                        continue;
                    //printf("ret %i, %i %i\n",ret,outBufPos/sizeof(gr_complex), noutput_items);

                    cnt+=ret;
                    if (cnt>rxRate*5) {
                        cnt=0;
                        mutex.lock();
                        r2t2ServerMsg->set_command(R2T2Proto::R2T2Message_Command_ALIVE);
                        sendR2T2Msg(); 
                        mutex.unlock();
                    }

                    return ret;
                }
            }
            return -1;
        }

        void source_impl::setRxFreq(int f) {
            printf ("set freq %i\n",f);
            mutex.lock();
            r2t2ServerMsg->set_rxfreq(f);
            sendR2T2Msg(); 
            mutex.unlock();
        }
        
		void source_impl::setAntenna(int antenna) {
            printf ("set antenna %i\n",antenna);
            mutex.lock();
            r2t2ServerMsg->set_antenna(antenna);
            sendR2T2Msg(); 
            mutex.unlock();
        }

        void source_impl::setRate(int rate) {
            printf ("set rate %i\n",rate);
            rxRate = rate;
            mutex.lock();
            r2t2ServerMsg->set_rxrate(rate);
            sendR2T2Msg(); 
            mutex.unlock();
        }

        void source_impl::setAtt(int n) {
            printf ("set att %i\n",n);
            R2T2Proto::R2T2Message_Gain *msgAtt = new R2T2Proto::R2T2Message_Gain();
            msgAtt->set_adc(0);
            msgAtt->set_gain(n);
            mutex.lock();
            r2t2ServerMsg->set_allocated_att(msgAtt);
            sendR2T2Msg(); 
            mutex.unlock();
        }

        void source_impl::setGain(int n) {
            printf ("set gain %i\n",n);
            R2T2Proto::R2T2Message_Gain *msgGain = new R2T2Proto::R2T2Message_Gain();
            msgGain->set_adc(0);
            msgGain->set_gain(n);
            mutex.lock();
            r2t2ServerMsg->set_allocated_gain(msgGain);
            sendR2T2Msg(); 
            mutex.unlock();
        }

        void source_impl::sendR2T2Msg() {

            if (r2t2ServerMsg->ByteSize() == 0)
                return;

            std::ostringstream out;
            r2t2ServerMsg->SerializeToOstream(&out);

            strcpy((char*)outBuf, "R2");
            outBuf[2] = out.str().size() & 0xff;
            outBuf[3] = (out.str().size() >> 8) & 0xff;

            memcpy(outBuf+4, out.str().data(), out.str().size());

            send(r2t2Socket, outBuf, out.str().size()+4, 0);

            r2t2ServerMsg->Clear();
            alive = 5;
        }

        int source_impl::openSocket (const char *host, unsigned short port) {

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
        int source_impl::get_port(void)
        {
            sockaddr_in name;
            socklen_t len = sizeof(name);
            int ret = getsockname( r2t2Socket, (sockaddr*)&name, &len );
            if( ret ) {
                perror("source_impl getsockname");
                return -1;
            }
            return ntohs(name.sin_port);
        }

        void source_impl::closeSocket(int socket) {
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

        void source_impl::disconnect()
        {
            closeSocket(r2t2Socket);
            return;
        }


    } /* namespace r2t2 */
} /* namespace gr */

