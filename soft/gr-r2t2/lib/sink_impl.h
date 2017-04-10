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

#ifndef INCLUDED_R2T2_SINK_IMPL_H
#define INCLUDED_R2T2_SINK_IMPL_H

#include <r2t2/sink.h>
#include "source_impl.h"
#include "r2t2.pb.h"

namespace gr {
    namespace r2t2 {

        class sink_impl : public sink
        {
            private:
                void sendR2T2Msg();
                void closeSocket(int socket);
                int openSocket (const char *host, unsigned short port);
                void disconnect();
                int get_port();

                size_t	d_itemsize;
                bool d_connected;
                int r2t2Socket;
                R2T2Proto::R2T2Message *r2t2ServerMsg, *r2t2Msg;
                boost::mutex mutex;
                uint8_t outBuf[64*1024];
                uint8_t inBuf[2048];
                int connected = 0;
                int dataReq = 0;
                uint32_t lastAck=0;

            public:
                sink_impl(size_t itemsize, const char *host, unsigned short port, int freq, int rate);
                ~sink_impl();

                // Where all the action really happens
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                void setTxFreq(int f);
                void setRate(int rate);
        };

    } // namespace r2t2
} // namespace gr

#endif /* INCLUDED_R2T2_SINK_IMPL_H */
