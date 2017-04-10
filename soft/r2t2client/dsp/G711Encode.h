#ifndef _G711ENCODE_H_
#define _G711ENCODE_H_

#include "types.h"
#include "ProcessBlock.h"

class G711Encode: public ProcessBlock 
{

	public:
		G711Encode(std::string name);
		~G711Encode();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);

	private:
		int process(float* in, int cnt, int8_t *out);
};

#endif
