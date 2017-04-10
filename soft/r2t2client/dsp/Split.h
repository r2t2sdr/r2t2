#ifndef _SPLIT_H_
#define _SPLIT_H_

#include "types.h"
#include "ProcessBlock.h"

class Split: public ProcessBlock 
{

	public:
		Split(std::string name);
		~Split();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);

};

#endif
