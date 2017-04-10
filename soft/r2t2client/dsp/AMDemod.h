#ifndef _AMDEMOD_H_
#define _AMDEMOD_H_

#include "types.h"
#include "ProcessBlock.h"

class AMDemod: public ProcessBlock 
{

	public:
		AMDemod(std::string name);
		~AMDemod();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);

	private:
		int process(cfloat_t* in, int cnt);
};

#endif
