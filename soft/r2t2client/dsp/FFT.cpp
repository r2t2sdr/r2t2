#include <QDebug>
#include <string.h>
#include <volk/volk.h>
#include "FFT.h"
#include "types.h"
#include "lib.h"
#include "config.h"

FFT::FFT(std::string name, int size, bool inv) : ProcessBlock(name, 1, 1), size(size), inverse(inv) {

	setInputType(0, typeid(cfloat_t));
	kissState = kiss_fft_alloc(size, inverse, 0, 0);
	window.clear();
	for (int i=0;i<size;i++) 
		//window.push_back(0.5  - 0.5  * cos (2 * M_PI / size * i));  // Hanning window
		// window.push_back(0.35875  - 0.48829*cos(2*M_PI/size*i) + 0.14128*cos(4*M_PI/size*i) - 0.01168*cos(6*M_PI/size*i));  // Blackman Herris
		window.push_back(0.3625819  - 0.4891775*cos(2*M_PI/size*i) + 0.1365995*cos(4*M_PI/size*i) - 0.0106411*cos(6*M_PI/size*i));  // Blackman Nuttall 
}


FFT::~FFT() {
	kiss_fft_cleanup();
}

int FFT::setSize(int s) {
    if (s<1024) 
        s=1024;
    else if (s<2048) 
        s=2048;
    else s=4096;

	inBuf[0].reset();
    if (s==size) {
		work = 1;
        return size;
	}

    size = s;

	kiss_fft_cleanup();
	kissState = kiss_fft_alloc(size, inverse, 0, 0);
    window.clear();
	for (int i=0;i<size;i++) 
		// window.push_back(0.5  - 0.5  * cos (2 * M_PI / size * i));  // Hanning window
		// window.push_back(0.35875  - 0.48829*cos(2*M_PI/size*i) + 0.14128*cos(4*M_PI/size*i) - 0.01168*cos(6*M_PI/size*i));  // Blackman Herris
		window.push_back(0.3625819  - 0.4891775*cos(2*M_PI/size*i) + 0.1365995*cos(4*M_PI/size*i) - 0.0106411*cos(6*M_PI/size*i));  // Blackman Nuttall 
	work = 1;
    return size;
}

int FFT::process(cfloat_t* in0, int in0Cnt, cfloat_t* out) {

	if (in0Cnt < size)
		return 0;
	volk_32fc_32f_multiply_32fc((cfloat*)in0, (cfloat*)in0, &window[0], size);
	kiss_fft(kissState, (kiss_fft_cpx*)in0, (kiss_fft_cpx*)out);
	return size;
}


int FFT::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	int minBuf = preReceive(buf, input, recursion);
	if (minBuf<0)
		return 0;

	if (inBuf[0]->size() < (unsigned int)size) 
		return 0;

	if (!work) {
		inBuf[0].reset();
		return 0;
	}

    auto outBuf = std::make_shared<ProcessBuffer> (size, typeid(cfloat_t));

    int nElements = process((cfloat_t*)**(inBuf[0]), inBuf[0]->size(), (cfloat_t*)**(outBuf));

	inBuf[0]->processed(nElements);
	outBuf->setSize(nElements);

	setGlobTime(name.data());
	send(outBuf, 0, recursion+1);
	work = 0;
	return 0;
}
