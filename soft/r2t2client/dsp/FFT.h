#ifndef _FFT_H_
#define _FFT_H_

#include "types.h"
#include "ProcessBlock.h"
#include "kiss_fft.h"
#include "config.h"
#include "window.h"

class FFT : public ProcessBlock 
{
	public: 

		FFT (std::string name, int size, bool inv=false, FFTWindow fftWindow = None, int inBufferFactor = 1, int outBufferFactor = 1) ;
		~FFT ();

		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion=0);
        int setSize(int);
        void setWindow(FFTWindow);

	private:
		int process(cfloat_t* in0, int in0Cnt, cfloat_t* out, int outCnt);
		std::vector<float> window;
		int size;
		bool inverse;
		kiss_fft_state *kissState;
		int work=0;
        FFTWindow fftWindow;
        int inBufferFactor, outBufferFactor;
        cfloat_t tmpOut[MAX_FFT_SIZE];
};

#endif
