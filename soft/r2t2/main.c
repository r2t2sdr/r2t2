#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "r2t2defs.h"
#include "reg.h"
#include "spi.h"
#include "si5344_init.h"
#include "socket.h"


#define VERSION "1.0"
#define PRE_DIVIDER  (16*2)


uint32_t sampleRate = 122880000;

uint32_t phaseInc(uint32_t f) {
	return (uint32_t)(pow(2,30)*f/sampleRate); 
}

void setTxFreq(uint32_t f) {
	setMem(REG_TX_FREQ, phaseInc(f));
}

void setTxRate(uint32_t f) {
	uint32_t div = sampleRate/f/2;
	printf ("divider %i,  rate %i\n",div, sampleRate/div/2);
	if (div<125 || div>3125) {
		printf ("invalid rate %i, ignored\n",div);
		return;
	}
	setMem(REG_TX_DIVIDER, div);
}

void setRxFreq(int rx, uint32_t f) {
	setMem(REG_RX_FREQ_BASE + REG_RX_OFFSET*rx, phaseInc(f));
}

void setRxRate(int rx, uint32_t f) {
	uint32_t div = sampleRate/f/PRE_DIVIDER;
	printf ("divider %i,  rate %i\n",div, sampleRate/div/PRE_DIVIDER);
	if (div<4 || div>8192) {
		printf ("invalid rate %i, ignored\n",div);
		return;
	}
	setMem(REG_RX_DIVIDER_BASE + REG_RX_OFFSET*rx, div);
}

void usage(char *proc) {
	printf ("usage: %s\n", proc);
	printf ("        -c rx              select rx [0..8]\n");
	printf ("        -r rxfreq          set rx freq\n");
	printf ("        -a rx rate         set rx rate\n");
	printf ("        -t rxfreq          set tx freq\n");
	printf ("        -b tx rate         set rx rate\n");
	printf ("        -s samplerate      use sample rate\n");
	printf ("        -u                 init clock chip\n");
	printf ("        -p                 init adc and dac\n");
	printf ("        -g gain (-9 ..32)  rx 1 gain\n");
	printf ("        -k gain (-9 ..32)  rx 2 gain\n");
	printf ("        -i att  (0..31)    rx 1 att\n");
	printf ("        -j att  (0..31)    rx 2 att\n");
	printf ("        -e                 read rx data to stdout\n");
	printf ("        -f                 write 2 tone signal to tx data\n");
	printf ("        -d reg             select reg\n");
	printf ("        -l cnt             read from selected reg to reg+cnt\n");
	printf ("        -w val             write val to selected reg\n");
	printf ("        -h                 this usage\n");
}

uint32_t getOpt(const char* str) {
	char *endp;
	if (str[0]=='0' && str[1]=='x') 
		return strtoul(str+2, &endp, 16);
	if (str[0]=='0' && str[1]=='b') 
		return strtoul(str+2, &endp, 2 );
	else
		return (atof(optarg));
}

int main(int argc, char *argv[]) {
	int i,n;
	initSpi();
	uint32_t reg=0;
	uint32_t val;
	int rx = 1;

	while ((i = getopt(argc, argv, "+r:a:b:t:s:pg:k:i:j:efud:w:l:hc:")) != EOF) {
		switch (i) {
			case 'c':
				rx = getOpt(optarg);
				if (rx<0 || rx>8) { 
					printf ("invalid rx %i\n",rx);
					rx=1;
				}
				break;
			case 'r':
				setRxFreq(rx, getOpt(optarg));
				break;
			case 'a':
				setRxRate(rx, getOpt(optarg));
				break;
			case 'b':
				setTxRate(getOpt(optarg));
				break;
			case 't':
				setTxFreq(getOpt(optarg));
				break;
			case 's':
				sampleRate = getOpt(optarg);
				break;
			case 'u':
				printf ("init clock chip to %i\n", sampleRate);
				if (sampleRate == 125e6)
					initClock(0);
				else if (sampleRate == 122.88e6)
					initClock(1);
				else if (sampleRate == 62.5e6)
					initClock(2);
				else 
					printf ("unknown clock freq: usable 125e6, 122.88e6, 62.5e6\n");
				break;
			case 'p':
				initDAC();
				initADC();
				break;
			case 'g':
				setRxGain(atoi(optarg),0);
				break;
			case 'k':
				setRxGain(atoi(optarg),1);
				break;
			case 'i':
				setRxAtt(atoi(optarg),0);
				break;
			case 'j':
				setRxAtt(atoi(optarg),1);
				break;
			case 'e':
				readSocket("rad0");
				break;
			case 'f':
				writeSocket("rad0");
				break;
			case 'd':
				reg = getOpt(optarg);
				break;
			case 'l':
				if (!reg) {
					printf("no reg selected\n");
					return 0;
				}
				for (n=0;n<getOpt(optarg);n++)
					printf ("%08x: %08x\n",reg+n*4, readMem(reg+n*4));

				break;
			case 'w':
				if (!reg) {
					printf("no reg selected\n");
					return 0;
				}
				val = getOpt(optarg);
				printf ("set reg %08x to %08x\n", reg, val);
				setMem(reg, val);
				break;
			case 'h':
			default:
				usage(argv[0]);
				return -1;
		}
	}
	return 0;
}
