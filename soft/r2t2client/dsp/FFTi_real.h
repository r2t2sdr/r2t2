#ifndef _FFTi_real_REAL_H_
#define _FFTi_real_REAL_H_

#include "types.h"
#include "ProcessBlock.h"
#include "kiss_fftr.h"
#include "config.h"
#include "window.h"

class FFTi_real : public ProcessBlock 
{
	public: 

		FFTi_real (std::string name, int size, FFTWindow fftWindow = None, int bufferFactor = 1) ;
		~FFTi_real ();

		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion=0);
        int setSize(int);
        void setWindow(FFTWindow);

	private:
		int process(cfloat_t* in0, int in0Cnt, float* out, int outCnt);
		std::vector<float> window;
		int size;
		kiss_fftr_state *kissState;
		int work=0;
        FFTWindow fftWindow;
        int bufferFactor;
        cfloat_t tmpC[MAX_FFT_SIZE];
        float tmpR[MAX_FFT_SIZE];
};

#endif
