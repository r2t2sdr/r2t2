#include <QDebug>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#include "r2t2defs.h"
#include "reg.h"
#include "spi.h"
#include "si5344_init.h"
#include "socket.h"
#include "r2t2.h"
#include "lib.h"


#define PRE_DIVIDER_TX  (2)
#define SCALE_32 		2147483647.0 
#define SCALE_16 		(32767.0*0.9) 

//#define NOHW

R2T2::R2T2(uint32_t clk) : clk(clk) {
	int n=0;
	if (clk <= 62500000) {
		clk = 6250000;
		n = 2;
	} else if (clk <= 122880000) {
		clk = 122880000;
		n = 1;
	} else { 
		clk = 125000000;
		n = 0;
	}

	printf("using clock %i\n",clk);

#ifdef NOHW
	return;
#endif

	initSpi();
	initClock(n);
	initDAC();
	initADC();

	// reset adc part in fpga
	setMem(0xf8000240, 2);
	setMem(0xf8000240, 0);

	for (int rx=0;rx<MAX_RX;rx++) {
		setRxFreq(rx, 7000000);
		setInput(rx, 1);
	}
	setTxFreq(700000);
	setTxRate(48000);
	setGain(0,0);
	setGain(1,0);
	setAtt(0,0);
	setAtt(1,0);

	r2t2Socket = connectR2T2("rad0");
}

R2T2::~R2T2() {
#ifdef NOHW
	return;
#endif
	if (r2t2Socket >= 0)
		disconnectR2T2(r2t2Socket);
}

uint32_t R2T2::phaseInc(uint32_t f) {
	return (uint32_t)(pow(2,30)*f/clk);
}
        
void R2T2::setRxFreq(uint32_t rx, uint64_t rxFreq) {
   PDEBUG(MSG2, "setRxFreq %i %i", rx, (uint32_t)rxFreq);

	if (rx>=MAX_RX) {
		assert(0);
		return;
	}
#ifdef NOHW
	return;
#endif
	setMem(REG_RX_FREQ_BASE + 4*rx, phaseInc(rxFreq));
}

void R2T2::setTxFreq(uint64_t txFreq) {
    PDEBUG(MSG2, "setTxFreq %i %i", txFreq);
#ifdef NOHW
	return;
#endif
	setMem(REG_TX_FREQ, phaseInc(txFreq));
}


void R2T2::setTxRate(uint32_t rate) {
	uint32_t div = clk/rate/PRE_DIVIDER_TX;

    PDEBUG(MSG2, "setTxRate %i", rate);
	if (div<125 || div>3125) {
		printf ("invalid rate %i, ignored\n",div);
		return;
	}
#ifdef NOHW
	return;
#endif
	setMem(REG_TX_DIVIDER, div);
}

void R2T2::setAtt(uint32_t adc, int att) {
    PDEBUG(MSG2, "setAtt %i %i", adc, att);
#ifdef NOHW
	return;
#endif
	setRxAtt(att, adc);
}


void R2T2::setGain(uint32_t adc, int gain) {
    PDEBUG(MSG2, "setGain %i %i", adc, gain);
#ifdef NOHW
	return;
#endif
	setRxGain(gain, adc);
}

// input 1: I (Ant 0)
//       2: Q (Ant 1)
//       3: IQ
//       else: disable RX
void R2T2::setInput(uint32_t rx, uint32_t input) {
    PDEBUG(MSG2, "setInput %i %i", rx, input);
	if (rx>=MAX_RX) {
		assert(0);
		return;
	}
	if (input>=1 && input <= 3) 
		setMem(REG_RX_INPUT_BASE + REG_RX_OFFSET*rx, input-1);
	else
		setMem(REG_RX_INPUT_BASE + REG_RX_OFFSET*rx, 0x80000000);
	setMem(REG_RX_INPUTACT_BASE + REG_RX_OFFSET*rx, 2);
}

int R2T2::recv(int32_t **out, int maxCnt, int &cnt, int &first, int &last) {
	int ret;
	uint32_t len = maxCnt*sizeof(float);

	if (len > sizeof(rxSampleBuf))
		len = sizeof(rxSampleBuf);
#ifdef NOHW
	return -1;
#endif

	ret = readSocket(r2t2Socket, (uint8_t*)rxSampleBuf, len);
	if (ret <= 0)
		return -1;

	assert((ret & 7) == 0); // IQ-float

	int32_t *sampleBuf = rxSampleBuf + R2T2_HEADER_SIZE / sizeof(rxSampleBuf[0]);

	int nSamples = (ret - R2T2_HEADER_SIZE)/8;
	uint8_t rxInfo = *(uint32_t*)sampleBuf >> 24;
	int rx = rxInfo & 15;
	last = (rxInfo & 0x20)>0;
	first = (rxInfo & 0x10)>0;

	if (nSamples > MAX_RX_SAMPLES) {
		PDEBUG(MSG4, "rxData samples error %i %i", rx, nSamples);
		assert(0);
		nSamples = MAX_RX_SAMPLES;
	}

	*out =  sampleBuf;
	cnt = nSamples*2;
    PDEBUG(MSG4, "rxData %i %i", rx, cnt);
	return rx;
}

int R2T2::xmit(float *in, int cnt) {
	if (cnt > MAX_TX_SAMPLES) {
		assert(0);
		cnt = MAX_TX_SAMPLES;
	}

	for (int k=0; k < cnt; k++) {
		if (in[k]>1.0)
			in[k]=1.0;
		if (in[k]<-1.0)
			in[k]=-1.0;
		txSampleBuf[k] = (int16_t)(in[k] * SCALE_16);
	}
#ifdef NOHW
	return 0;
#endif
	int ret = writeSocket(r2t2Socket, (uint8_t*)txSampleBuf, cnt*sizeof(int16_t));
    PDEBUG(MSG3, "txData %i %i", cnt, ret);
	return ret;
}
