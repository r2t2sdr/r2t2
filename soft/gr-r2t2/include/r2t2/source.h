/* -*- c++ -*- */
/* 
 * Copyright 2013 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_R2T2_SOURCE_H
#define INCLUDED_R2T2_SOURCE_H

#include <r2t2/api.h>
#include <gnuradio/sync_block.h>

#define RX_CLOCK	122880000LL

namespace gr {
    namespace r2t2 {

	/*!
	 * \brief <+description of block+>
	 * \ingroup r2t2
	 *
	 */
	class R2T2_API source : virtual public gr::sync_block
	{
	    public:
		typedef boost::shared_ptr<source> sptr;

		/*!
		 * \brief Return a shared_ptr to a new instance of r2t2::source.
		 *
		 * To avoid accidental use of raw pointers, r2t2::source's
		 * constructor is in a private implementation
		 * class. r2t2::source::make is the public interface for
		 * creating new instances.
		 */
		static sptr make(size_t itemsize, const char *host="localhost", unsigned short port=8000, int rxfreq=7000000, int rate=48000, int att=0, int gain=0, int antenna=0);
		virtual void setAntenna(int antenna)=0;
		virtual void setRxFreq(int f)=0;
		virtual void setRate(int rate)=0;
		virtual void setAtt(int)=0;
		virtual void setGain(int)=0;
	};

    } // namespace r2t2
} // namespace gr

#endif /* INCLUDED_R2T2_SOURCE_H */

