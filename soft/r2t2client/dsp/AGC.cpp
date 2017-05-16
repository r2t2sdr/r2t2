#include <QDebug>
#include <string.h>

#include "AGC.h"
#include "types.h"
#include "lib.h"
#include "config.h"

static float decVal[] = {0, 0.00001, 0.0001, 0.001, 0.01};
static float attVal[] = {0, 0.00001, 0.0001, 0.001, 0.01};

AGC::AGC(std::string name) : ProcessBlock(name, 1, 1) {

	setInputType(0, typeid(float));
    att = 1-0.01;
	dec = 1+0.0001;
	gain = 100000;
	ref = 0.8;
	peek = 0;
}


AGC::~AGC() {
}

void AGC::setDec(int n) {
	dec = 1+decVal[checkRange(n, 0, arraysize(decVal)-1)];
	setAtt(n==0 ? 0:3);
}

void AGC::setAtt(int n) {
	att = 1-attVal[checkRange(n, 0, arraysize(attVal)-1)];
}

int AGC::process(float* in0, int in0Cnt) {
//	qDebug() << "in " << *in0;
	for (int i=0;i<in0Cnt;i++) {

		float in = in0[i]*gain;
		in0[i] = in; 

		float absIn = fabs(in);
		if (absIn>ref)
			gain *= ref/absIn;
			// gain *= att;
		if (absIn<ref)
			gain *= dec;

	}
	return in0Cnt;
}


int AGC::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	int minBuf = preReceive(buf, input, recursion);
	if (minBuf<0)
		return 0;

    process((float*)**(inBuf[0]), inBuf[0]->size());

	setGlobTime(name.data());
	send(inBuf[0], 0, recursion+1);
	inBuf[0].reset();
	return 0;
}
