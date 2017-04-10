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

#ifndef INCLUDED_R2T2_SOURCE_IMPL_H
#define INCLUDED_R2T2_SOURCE_IMPL_H

#include <r2t2/source.h>
#include "r2t2.pb.h"

namespace gr {
  namespace r2t2 {

    class source_impl : public source
	  {
		  private:
              void sendR2T2Msg();
			  void closeSocket(int socket);
			  int openSocket (const char *host, unsigned short port);
			  void disconnect();

			  uint8_t outBuf[20*64*1024];
			  uint8_t inBuf[4096];
              int r2t2Socket;
              R2T2Proto::R2T2Message *r2t2ServerMsg, *r2t2Msg;
              boost::mutex mutex;
			  int get_port();
              int alive=0;
              int connected=0;
              int outBufPos=0;
              int rxRate=0;

		  public:
			  source_impl(size_t istemsize, const char *host, unsigned short port, int rxfreq, int rate, int att, int gain, int antenna);
			  ~source_impl();

			  // Where all the action really happens
			  int work(int noutput_items,
					  gr_vector_const_void_star &input_items,
					  gr_vector_void_star &output_items);

			  void setRxFreq(int f);
			  void setRate(int rate);
			  void setAtt(int att);
			  void setGain(int gain);
			  void setAntenna(int antenna);
	  };

  } // namespace r2t2
} // namespace gr

#endif /* INCLUDED_R2T2_SOURCE_IMPL_H */

