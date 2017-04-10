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


#ifndef INCLUDED_R2T2_SINK_H
#define INCLUDED_R2T2_SINK_H

#include <r2t2/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace r2t2 {

    /*!
     * \brief <+description of block+>
     * \ingroup r2t2
     *
     */
    class R2T2_API sink : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<sink> sptr;

      static sptr make(size_t itemsize, const char *host, unsigned short port, int freq, int rate);
    };

  } // namespace r2t2
} // namespace gr

#endif /* INCLUDED_R2T2_SINK_H */

