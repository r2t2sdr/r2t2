#ifndef _CONFIG_H_
#define _CONFIG_H_

#define VERSION "1.0"

#define QTRADIO_SERVER_PORT 11000	
#define QTRADIO_DSP_PORT	8000
#define R2T2_PORT	        8008
#define R2T2_SERVER_PORT	8000
#define CONN_TIMEOUT        20

#define AUDIO_RATE  8000

#define DSP_BUFFER_SIZE 	4096
#define MAX_FFT_SIZE        4096

#define HEADER_SIZE     12

#define BUFFER_HEADER_SIZE          15
#define AUDIO_BUFFER_HEADER_SIZE    5

#define SPECTRUM_BUFFER     0
#define AUDIO_BUFFER        1
#define BANDSCOPE_BUFFER    2
#define RTP_REPLY_BUFFER    3
#define ANSWER_BUFFER       4
    
#define HEADER_VERSION      2
#define HEADER_SUBVERSION   1

#define amplOffset         10
#define amplFFTOffset      18

#define CW_OFFSET           600

#endif
