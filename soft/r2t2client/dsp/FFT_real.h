#ifndef _FFT_real_REAL_H_
#define _FFT_real_REAL_H_

#include "types.h"
#include "ProcessBlock.h"
#include "kiss_fftr.h"
#include "config.h"
#include "window.h"

class FFT_real : public ProcessBlock 
{
	public: 

		FFT_real (std::string name, int size, FFTWindow fftWindow = None, int bufferFactor = 1);
		~FFT_real ();

		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion=0);
        int setSize(int);
        void setWindow(FFTWindow);

	private:
		int process(float* in0, int in0Cnt, cfloat_t* out, int outCnt);
		std::vector<float> window;
		int size;
		kiss_fftr_state *kissState;
		int work=0;
        FFTWindow fftWindow;
        int bufferFactor;
        float tmpR[MAX_FFT_SIZE];
        cfloat_t tmpC[MAX_FFT_SIZE];
};

#endif
