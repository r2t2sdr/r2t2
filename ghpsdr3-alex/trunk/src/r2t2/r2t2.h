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


#if !defined __R2T2_H__
#define      __R2T2_H__

typedef int (*R2T2_CB)(void *buf, int bsize, void *data) ;


int r2t2_init (int r2t2_bw = 48000, long long r2t2_f = 7050000LL);
int r2t2_set_frequency (int rx, long long f);
int r2t2_set_bandwidth (int rx, long long b);
int r2t2_set_attenuator (int attDb);
int r2t2_set_antenna_input (int n);
int r2t2_set_preamp (int newstatus);
int r2t2_get_preamp (void);
int r2t2_start_asynch_input (R2T2_CB cb, void *pud);
int r2t2_stop_asynch_input ();
int r2t2_deinit (void);

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#endif
