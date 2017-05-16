#include <QDebug>
#include <string.h>
#include <volk/volk.h>
#include "FFTi_real.h"
#include "types.h"
#include "lib.h"
#include "config.h"


FFTi_real::FFTi_real(std::string name, int size, FFTWindow win, int bufferFactor) : 
    ProcessBlock(name, 1, 1), 
    size(size), 
    fftWindow(win), 
    bufferFactor(bufferFactor)
{

	setInputType(0, typeid(cfloat_t));
	kissState = kiss_fftr_alloc(size, true, 0, 0);
    setWindow(fftWindow);
    work = 1;
}


FFTi_real::~FFTi_real() {
	kiss_fftr_free(kissState);
}

int FFTi_real::setSize(int s) {
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
	kissState = kiss_fftr_alloc(size, true, 0, 0);
    setWindow(fftWindow);
	work = 1;
    return size;
}
        
void FFTi_real::setWindow(FFTWindow win) {
    fftWindow = win;
    calcWindow(fftWindow, window, size, true);
    for(int i=0;i<size;i++) 
        window[i]/=size;
}

int FFTi_real::process(cfloat_t* in0, int in0Cnt, float* out, int outCnt) {

    if (in0Cnt < size)
        return 0;

    int offset = (size - outCnt)/2;
    kiss_fftri(kissState, (kiss_fft_cpx*)in0, (kiss_fft_scalar*)tmpR);
    for (int i=0;i<outCnt;i++)
        out[i] = tmpR[i+offset]*window[i+offset]; 

    return outCnt;
}


int FFTi_real::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
    int minBuf = preReceive(buf, input, recursion);
    if (minBuf<0) {
        return 0;
    }

    if (inBuf[0]->size() < (unsigned int)(size/2+1)) {
        return 0;
    }

    if (!work) {
        inBuf[0].reset();
        return 0;
    }

    int outSize = size/bufferFactor;

    while (inBuf[0]->size() >= (unsigned int)(size/2+1)) {
        auto outBuf = std::make_shared<ProcessBuffer> (outSize, typeid(float));

        int nElements = process((cfloat_t*)**(inBuf[0]), size, (float*)**(outBuf), outSize);

        inBuf[0]->processed((size/2+1));
        outBuf->setSize(nElements);

        setGlobTime(name.data());
        send(outBuf, 0, recursion+1);
    }
    return 0;
}
