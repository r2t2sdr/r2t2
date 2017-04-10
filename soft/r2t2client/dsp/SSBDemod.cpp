#include "SSBDemod.h"
#include "types.h"
#include "lib.h"


SSBDemod::SSBDemod(std::string name) : ProcessBlock(name, 1, 1) {
    setInputType(0, typeid(cfloat_t));
}

SSBDemod::~SSBDemod() {
}


int SSBDemod::process(cfloat_t* in, int cnt) {
	int processed = 0;
	float *out = (float*)in;
	while (processed < cnt) {
		cfloat_t x = *in++;
		*out++ = x.re + x.im;
		processed++;
	}
	return processed;
}

int SSBDemod::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	int minBuf = preReceive(buf, input, recursion);
	if (minBuf<0)
		return 0;

	int nElements = process((cfloat_t*)**(inBuf[0]), inBuf[0]->size());

	inBuf[0]->setType(typeid(float));
	inBuf[0]->setSize(nElements);

	setGlobTime(name.data());
	send(inBuf[0], 0, recursion+1);
	inBuf[0].reset();
	return 0;
}
