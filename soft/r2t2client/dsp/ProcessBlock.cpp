#include <cstddef>
#include <memory>
#include <assert.h>
#include <string.h>
#include <climits>
#include <sstream>
#include "lib.h"
#include "dsp/ProcessBuffer.h"
#include "dsp/ProcessBlock.h"

ProcessBlock::ProcessBlock(std::string s, uint32_t input, uint32_t output) :nIn(input), nOut(output), name(s) {

	memset(next, 0, sizeof(next));
	for (uint32_t i=0;i<MAX_PIN;i++)
		inType[i] = NULL;

};  

ProcessBlock::~ProcessBlock() {
};  


int ProcessBlock::connect(uint32_t pin, ProcessBlock *dest, uint32_t input) {
	if (pin >= MAX_PIN) {
		assert(0);
		return -1;
	}

	next[pin].dest = dest;
	next[pin].input = input;

	return 0;
}

int ProcessBlock::disconnect(uint32_t output) {

	if (output >= nOut) {
		assert(0);
		return -1;
	}

	next[output].dest = NULL;
	next[output].input = 0;
	
	return 0;
}
		
void ProcessBlock::setInputType(int input, const std::type_info &type) {
	if ((uint32_t)input<MAX_PIN)
		inType[input]=&type;
	else
		assert(0);
}

int ProcessBlock::append(std::shared_ptr<ProcessBuffer> buf, uint32_t input) {
	if (!buf)
		return -1;

	if (input >= nIn) {
		assert(0);
		return -1;
	}

	if (!inBuf[input] || inBuf[input]->size() == 0) 
		inBuf[input] = buf;
	else {
		inBuf[input]->append(buf.get());
	}

	return inBuf[input]->size();

}

int ProcessBlock::getShortestInput() {
	size_t min=UINT_MAX;
	int	minPos=-1;

	for (uint32_t i=0; i<nIn; i++) {

		if (!inBuf[i]) 
			return -1;
		if (min > inBuf[i]->size()) {
			min = inBuf[i]->size();
			minPos = i;
		}
	}
	return minPos;
}

std::string ProcessBlock::getName() {
	return name;
}

int ProcessBlock::preReceive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	ARG_UNUSED(recursion);
	if (!buf)
		return -1;

	if (!inType[input]) {
		assert(0);
	} else {
		if (buf->typeName() != inType[input]->name()) {
			printf("error: type not matching:  buf is %s, input is %s\n",buf->typeName(), inType[input]->name());
			assert(0);
		}
	}

	if (append(buf, input)<0)
		return -1;

#ifdef DEBUG 
	std::ostringstream debugStr;
	debugStr << getName() << " in" << input;

	if (inBuf[input])
		inBuf[input]->debug(debugStr.str(), recursion);
#endif

	return getShortestInput();
}

int ProcessBlock::send(std::shared_ptr<ProcessBuffer> buf, uint32_t out, int recursion) {
	int ret = -1;

	if (out >= nOut) {
		assert(0);
		return -1;
	}

	if (!buf || buf->size()==0)
		return 0;

	buf->debug(getName()+std::string(" out"), recursion);

	next_t *n = &next[out];
	if (n->dest) {
		ret = n->dest->receive(buf, n->input, recursion);
	} else {
		// not connected 
		printf ("not connected\n");
		assert(0);
	}

#if 0 // output stack position
	register int sp asm ("sp");
	blog("Stack %s, %08x",name.c_str(),sp);
#endif

	return ret; 
}

int ProcessBlock::inputProcess(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	int ret = -1;

	if (input >= nIn) {
		assert(0);
		return -1;
	}

	buf->debug(getName()+std::string(" in"), recursion);

	next_t *n = &next[input];
	if (n->dest) {
		ret = n->dest->receive(buf, n->input, recursion);
	} else
		// not connected 
		assert(0);

	return ret; 
}


