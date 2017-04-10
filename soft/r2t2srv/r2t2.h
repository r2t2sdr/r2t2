#ifndef _R2T2_H_
#define _R2T2_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_RX 			8 
#define MAX_TX			1
#define MAX_RX_SAMPLES 	2048
#define MAX_TX_SAMPLES 	(8*1024)

class R2T2 {
    private:
		uint32_t phaseInc(uint32_t f);
        uint32_t clk;
		int r2t2Socket;
		int32_t rxSampleBuf[MAX_RX_SAMPLES*2];
		int16_t txSampleBuf[MAX_TX_SAMPLES*2];

    public:
        R2T2(uint32_t clk);
        ~R2T2();

        void setRxFreq(uint32_t rx, uint64_t rxFreq);
        void setTxFreq(uint64_t txFreq);
        void setTxRate(uint32_t rate);
        void setAtt(uint32_t adc, int att);
        void setGain(uint32_t adc, int gain);
		void setInput(uint32_t rx, uint32_t input);
        int recv(int32_t**, int maxCnt, int &cnt, int &first, int &last);
        int xmit(float*, int cnt);
};


#endif
