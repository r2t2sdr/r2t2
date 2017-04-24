#ifndef _SMETER_H_
#define _SMETER_H_

#include "types.h"
#include "ProcessBlock.h"

class SMeter: public ProcessBlock 
{

	public:
		SMeter(std::string name);
		~SMeter();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);

	private:
		int process(cfloat_t* in, int cnt, float* out);
        float val;
};

#endif
