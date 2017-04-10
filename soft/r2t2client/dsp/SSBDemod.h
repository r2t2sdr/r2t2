#ifndef _SSBDEMOD_H_
#define _SSBDEMOD_H_

#include "types.h"
#include "ProcessBlock.h"

class SSBDemod: public ProcessBlock 
{

	public:
		SSBDemod(std::string name);
		~SSBDemod();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);

	private:
		int process(cfloat_t* in, int cnt);
};

#endif
