#include <QDebug>
#include "SMeter.h"
#include "types.h"
#include "lib.h"
#include "config.h"


SMeter::SMeter(std::string name) : ProcessBlock(name, 1, 1) {
    setInputType(0, typeid(cfloat_t));
}

SMeter::~SMeter() {
}

int SMeter::process(cfloat_t* in, int cnt, float *out) {
	int processed = 0;
	while (processed < cnt) {
		cfloat_t x = *in++;
        float cur = x.re*x.re+x.im*x.im;
        if (cur > val)
            val = cur;
		processed++;
        val *= 0.9995;
	}
    *out = log10(val)*10+amplOffset;
	return processed;
}

int SMeter::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	int minBuf = preReceive(buf, input, recursion);
	if (minBuf<0)
		return 0;

    auto outBuf = std::make_shared<ProcessBuffer> (1, typeid(float));

	process((cfloat_t*)**(inBuf[0]), inBuf[0]->size(), (float*)**outBuf);

	setGlobTime(name.data());
    outBuf->setSize(1);
	send(outBuf, 0, recursion+1);
	inBuf[0].reset();
	return 0;
}
