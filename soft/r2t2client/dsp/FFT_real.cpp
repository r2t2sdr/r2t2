#include <QDebug>
#include <string.h>
#include <volk/volk.h>
#include "FFT_real.h"
#include "types.h"
#include "lib.h"
#include "config.h"

FFT_real::FFT_real(std::string name, int size, FFTWindow win, int bufferFactor) : 
    ProcessBlock(name, 1, 1), 
    size(size), 
    fftWindow(win), 
    bufferFactor(bufferFactor)
{

	setInputType(0, typeid(float));
	kissState = kiss_fftr_alloc(size, false, 0, 0);
    setWindow(fftWindow);
    work = 1;
}


FFT_real::~FFT_real() {
	kiss_fftr_free(kissState);
}

int FFT_real::setSize(int s) {
    work = 0;
    if (s<=1024) 
        s=1024;
    else if (s<=2048) 
        s=2048;
    else s=4096;

	inBuf[0].reset();
    if (s==size) {
		work = 1;
        return size;
	}

    size = s;

	kiss_fftr_free(kissState);
	kissState = kiss_fftr_alloc(size, false, 0, 0);
    setWindow(fftWindow);
	work = 1;
    return size;
}
        
void FFT_real::setWindow(FFTWindow win) {
    fftWindow = win;
    calcWindow(fftWindow, window, size, false);
}

int FFT_real::process(float* in0, int in0Cnt, cfloat_t* out, int outCnt) {

    if (in0Cnt < size)
        return 0;

    for (int i=0;i<size;i++) 
        tmpR[i] = in0[i]*window[i]; 
    kiss_fftr(kissState, (kiss_fft_scalar*)tmpR, (kiss_fft_cpx*)out);

    return outCnt;
}


int FFT_real::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
    int minBuf = preReceive(buf, input, recursion);
    if (minBuf<0) {
        return 0;
    }

    if (inBuf[0]->size() < (unsigned int)size) {
        return 0;
    }

    if (!work) {
        inBuf[0].reset();
        return 0;
    }

    int outSize = size/2+1;

    while (inBuf[0]->size() >= (unsigned int)size) {
        auto outBuf = std::make_shared<ProcessBuffer> (outSize, typeid(cfloat_t));

        int nElements = process((float*)**(inBuf[0]), size, (cfloat_t*)**(outBuf), outSize);

        inBuf[0]->processed(size/bufferFactor);
        outBuf->setSize(nElements);

        setGlobTime(name.data());
        send(outBuf, 0, recursion+1);
    }
    return 0;
}
