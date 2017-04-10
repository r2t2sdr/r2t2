#ifndef _ROTATE_H_
#define _ROTATE_H_

#include "types.h"
#include "ProcessBlock.h"

#define ROTATOR_TAB_SIZE 	256

class Rotate: public ProcessBlock 
{

	public:
		Rotate(std::string name, float freq);
		~Rotate();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);
		void setFreq(float f);

	private:
		int process(cfloat_t* in, int cnt);
		cfloat_t ccosTab[ROTATOR_TAB_SIZE];
		float ph, dph;

};

#endif
