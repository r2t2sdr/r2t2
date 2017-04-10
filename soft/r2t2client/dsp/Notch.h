#ifndef _NOTCH_H_
#define _NOTCH_H_

#include "types.h"
#include "ProcessBlock.h"

class Notch: public ProcessBlock 
{

	public:
		Notch(std::string name, uint32_t sampleRate);
		~Notch();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);
		void setSampleRate(uint32_t rate);

	private:
		int process(float* in, int cnt);
		float z0=0,z1=0,z2=0;
		float notch_a1,notch_a2;
};

#endif
