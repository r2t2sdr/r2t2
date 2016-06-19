/**
* @file server.c
* @brief R2T2 server application
* @author Andrea Montefusco, IW0HDV
* @version 0.1
* @date 4/1/2012
*/


/* Copyright (C)
*  2012 - Andrea Montefusco, IW0HDV
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/

//#include <boost/program_options.hpp>
//#using namespace boost::program_options;
#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "client.h"
#include "listener.h"
#include "r2t2.h"
#include "r2t2defs.h"
#include "receiver.h"

struct R2T2Config cfg;

int debug_level = 0;

void print_help()
{
    cerr << 
    "    Allowed options:                                               " << std::endl <<
    "      -s [ --samplerate ] arg (=96000) samplerate in Samples/second" << std::endl <<
    "                                       (24000 | 48000 | 96000 | 192000)    " << std::endl <<
    "      -h [ --help ]                    print usage message         " << std::endl <<
    "      -d [ --debug ] arg (=0)          debug level                 " << std::endl <<
    std::endl;
}

int parseOptions (int argc, char **argv, R2T2Config &cfg)
{
  int c;
  int rc = 0;

  while (1) {
      static struct option long_options[] =
        {
          /* These options set a flag. */
          {"help",       no_argument,       0,  1 },
          {"debug",      required_argument, 0, 'd'},
          {"samplerate", required_argument, 0, 's'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "hd:s:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c) {
        case 's':
          printf ("option -c with value `%s'\n", optarg);
          cfg.sr = atoi(optarg);
          break;

        case 'd':
          printf ("option -d with value `%s'\n", optarg);
          debug_level = atoi(optarg);
          break;

        case 1:
        case '?':
        case 'h':
        default:
          /* getopt_long already printed an error message. */
          print_help();
          return -1;
          break;
        }
  }
  return rc;
}



int main(int argc, char* argv[]) {

    cfg.sr = 96000;
    debug_level = 3;

    if (parseOptions (argc, argv, cfg) != 0) {
        return 255;
    }

    cout << "Debug level:          " << debug_level <<  std::endl;
    cout << "Sample rate:          " << cfg.sr      <<  std::endl;

    // Init R2T2 
    if (r2t2_init (cfg.sr)) {
        cout << "No R2T2 hardware detected" << endl;
        return 255;
    } else {
        cout << "Press q <ENTER> to exit." << endl;
        init_receivers (&cfg);
        create_listener_thread();

        char ch;
        while((ch = getc(stdin)) != EOF) {
            if (ch == 'q') break;
        }
        return 0;
    }
}

