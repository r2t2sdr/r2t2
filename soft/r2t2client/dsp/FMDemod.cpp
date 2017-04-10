#include "FMDemod.h"
#include "types.h"
#include "lib.h"


FMDemod::FMDemod(std::string name) : ProcessBlock(name, 1, 1) {
    setInputType(0, typeid(cfloat_t));
}

FMDemod::~FMDemod() {
}

int FMDemod::process(cfloat_t* in, int cnt) {
	float v,d_re,d_im;
	int processed = 0;
	float *out = (float*)in;

	while (processed < cnt) {
		cfloat_t x = *in++;
		v = sqrt(x.re*x.re+x.im*x.im);
		x.re /= v;
		x.im /= v;
		d_re = x.re - re_l;
		d_im = x.im - im_l;
		re_l = x.re;
		im_l = x.im;
		*out++ = asin(d_im*x.re-d_re*x.im);
		processed++;
	}
	return processed;
}

int FMDemod::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
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
