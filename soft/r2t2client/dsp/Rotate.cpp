#include <QDebug>
#include "assert.h"
#include <string.h>
#include "Rotate.h"
#include "types.h"
#include "config.h"
#include "lib.h"

Rotate::Rotate(std::string name, float freq) : ProcessBlock(name, 1, 1) {
    setInputType(0, typeid(cfloat_t));

	for (int i=0;i<ROTATOR_TAB_SIZE;i++) {
		ccosTab[i].re = cos(M_PI*2*i/ROTATOR_TAB_SIZE);
		ccosTab[i].im = sin(M_PI*2*i/ROTATOR_TAB_SIZE);
	}
	ph = 0;
	setFreq(freq);
}

Rotate::~Rotate() {
}

int Rotate::process(cfloat_t* in0, int cnt) {
	cfloat_t in;
	for (int i=0;i<cnt;i++) {
		in.re = in0[i].re;
		in.im = in0[i].im;
		cmul(in, ccosTab[(int)ph]);
		in0[i].re = in.re;
		in0[i].im = in.im;
		ph += dph;
		if (ph >= ROTATOR_TAB_SIZE)
			ph -= ROTATOR_TAB_SIZE;
		if (ph < 0)
			ph += ROTATOR_TAB_SIZE;
	}
	return cnt;
}

void Rotate::setFreq(float f) {
    //qDebug() << name.data() << f;
    // assert(f<1.0);
    if (f>=1) {
        f=0;
		qDebug() << "illegal rotate freq" << f;
	}
    dph = f * ROTATOR_TAB_SIZE;
}

int Rotate::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
    int minBuf = preReceive(buf, input, recursion);
    if (minBuf<0)
        return 0;

    auto outBuf = std::make_shared<ProcessBuffer> (DSP_BUFFER_SIZE, typeid(cfloat_t));

    process((cfloat_t*)**(inBuf[0]), inBuf[0]->size());

	setGlobTime(name.data());
	send(inBuf[0], 0, recursion+1);
	inBuf[0].reset();
	return 0;
}
