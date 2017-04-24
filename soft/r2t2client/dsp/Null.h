#ifndef _NULL_H_
#define _NULL_H_

#include "types.h"
#include "ProcessBlock.h"

class Null: public ProcessBlock 
{

	public:
		Null(std::string name);
		~Null();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);

	private:
};

#endif
