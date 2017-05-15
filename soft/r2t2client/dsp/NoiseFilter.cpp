#include <QDebug>
#include <string.h>
#include <volk/volk.h>
#include "NoiseFilter.h"
#include "types.h"
#include "lib.h"
#include "config.h"

NoiseFilter::NoiseFilter(std::string name): ProcessBlock(name, 1, 1) {
	setInputType(0, typeid(cfloat_t));
}


NoiseFilter::~NoiseFilter() {
}


void NoiseFilter::setLevel(int v) {
    qDebug() << "Noise" << v;
    level = v*7;
}

int NoiseFilter::process(cfloat_t* in0, int in0Cnt) {
	int processed = 0;
	cfloat_t *out = in0;
    float d=0.1;

	while (processed < in0Cnt) {
        float v = std::fabs(in0->re) + std::fabs(in0->im);
        if (v > level) {
            *out++ = *in0++;
        } else {
            out->re = in0->re*d;
            out->im = in0->im*d;
            out++;
            in0++;
        }
		processed++;
	}
	return processed;
}


int NoiseFilter::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	int minBuf = preReceive(buf, input, recursion);
	if (minBuf<0)
		return 0;

	int nElements = process((cfloat_t*)**(inBuf[0]), inBuf[0]->size());

	inBuf[0]->setSize(nElements);

	setGlobTime(name.data());
	send(inBuf[0], 0, recursion+1);
	inBuf[0].reset();
	return 0;
}
