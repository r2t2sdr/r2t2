#include <QDebug>
#include "Notch.h"
#include "types.h"
#include "lib.h"


Notch::Notch(std::string name) : ProcessBlock(name, 1, 1) {
    setInputType(0, typeid(float));
}

Notch::~Notch() {
}

void Notch::setNotchFreq(double f) {
	// Mitra, S.K., "Design of Digital Notch Filters," Communications, IEEE Transactions on , vol.22, no.7, pp.964,970,
	// Jul 1974)
	float fs = f;
	float fb = 200;
	float fn = 1000;
	float omega0T = fn/(fs/2)*M_PI;
	float deltaT  = fb/(fs/2)*M_PI;
	notch_a2 = (1-tan(deltaT/2))/(1+tan(deltaT/2));
	notch_a1 = (1+notch_a2)*cos(omega0T);
}

#if 0
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
#endif

int Notch::process(float* in, int cnt) {
	int processed = 0;
	float *out = (float*)in;
	while (processed < cnt) {
        *out++ = (float)processNotch((float)(*in++));
		processed++;
	}
	return processed;
}

double Notch::processNotch(double in) {
	static double xst1 = 0;
	static double xst2 = 0;
	static double temp1 = 0;
	static double temp2 = 0;
	static double theta = 0; // initial value for notch frequency parameter.
	static double sth = sin(theta);
	static double cth = cos(theta);
	static double bw = 0.20*M_PI; // bandwidth parameter for notch filter.
	static double sth2 = sin(bw);
	static double cth2 = cos(bw);
	static double mu = 0.043; // adaptive filter step size.

	//
	// adaptive lattice notch filter:
	//
	temp1 = cth2*in-sth2*xst2;
	temp2 = sth2*in+cth2*xst2;
	double error = mu*(in + temp2); // notch filter output times step size.
	theta = theta - error*xst1; // coefficient update.
    qDebug() << theta;

#if 0
	double freqnew;
	if (theta>M_PI/2)
		freqnew = 0.5*(2*M_PI-(theta+M_PI/2))/M_PI; // instantaneous freq. estimate.
	else if (theta>-M_PI/2)
		freqnew = 0.5*(theta+M_PI/2)/M_PI;
	else
		freqnew = -0.5*(theta+M_PI/2)/M_PI;
	qDebug()  << freqnew*sampleRate;
#endif

	sth = sin(theta);
	cth = cos(theta);
	xst2 = sth*temp1+cth*xst1;
	xst1 = cth*temp1-sth*xst1;

	return in+temp2;
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
