#ifndef _FMDEMOD_H_
#define _FMDEMOD_H_

#include "types.h"
#include "ProcessBlock.h"

class FMDemod: public ProcessBlock 
{

	public:
		FMDemod(std::string name);
		~FMDemod();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);

	private:
	 	int process(cfloat_t* in, int cnt);
		float re_l,im_l;
};

#endif
