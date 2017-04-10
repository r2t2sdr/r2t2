#ifndef _FFTINTERPOL_H_
#define _FFTINTERPOL_H_

#include "types.h"
#include "config.h"
#include "ProcessBlock.h"

class FFTInterpol : public ProcessBlock 
{
	public: 
		FFTInterpol (std::string name, int size) ;
		~FFTInterpol ();

		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion=0);
		void setSize(int);

	private:
		int process(cfloat_t* in0, int in0Cnt);
		int size;
        uint8_t sBuffer[MAX_FFT_SIZE];
};

#endif
