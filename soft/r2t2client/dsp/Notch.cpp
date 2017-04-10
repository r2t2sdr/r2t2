#include "Notch.h"
#include "types.h"
#include "lib.h"


Notch::Notch(std::string name, uint32_t sampleRate) : ProcessBlock(name, 1, 1) {
    setInputType(0, typeid(float));
	setSampleRate(sampleRate);
}

Notch::~Notch() {
}

void Notch::setSampleRate(uint32_t sampleRate) {
	// Mitra, S.K., "Design of Digital Notch Filters," Communications, IEEE Transactions on , vol.22, no.7, pp.964,970,
	// Jul 1974)
	float fs = sampleRate;
	float fb = 200;
	float fn = 1000;
	float omega0T = fn/(fs/2)*M_PI;
	float deltaT  = fb/(fs/2)*M_PI;
	notch_a2 = (1-tan(deltaT/2))/(1+tan(deltaT/2));
	notch_a1 = (1+notch_a2)*cos(omega0T);
}

int Notch::process(float* in, int cnt) {
	int processed = 0;
	float *out = (float*)in;
	while (processed < cnt) {

		float b = z0 + z2;
		float a = notch_a1*z1 - notch_a2*b;

		z2 = z1;
		z1 = z0 + a;
		z0 = *in++;

		*out++ =  b - a; 
		processed++;
	}
	return processed;
}

int Notch::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	int minBuf = preReceive(buf, input, recursion);
	if (minBuf<0)
		return 0;

	process((float*)**(inBuf[0]), inBuf[0]->size());

	setGlobTime(name.data());
	send(inBuf[0], 0, recursion+1);
	inBuf[0].reset();
	return 0;
}
