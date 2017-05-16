#ifndef _NOISE_FILTER_H_
#define _NOISE_FILTER_H_

#include "types.h"
#include "ProcessBlock.h"


class NoiseFilter : public ProcessBlock 
{
	public: 

		NoiseFilter (std::string name) ;
		~NoiseFilter ();
        void setLevel(int);

		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion=0);

	private:
		int process(cfloat_t* in0, int in0Cnt);
        float max = 0;
        float lmax = 0;
        int level = 0;
};

#endif
