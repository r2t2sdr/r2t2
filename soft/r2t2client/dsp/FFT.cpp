#include <QDebug>
#include <string.h>
#include <volk/volk.h>
#include "FFT.h"
#include "types.h"
#include "lib.h"
#include "config.h"


FFT::FFT(std::string name, int size, bool inv, FFTWindow win, int inBufferFactor, int outBufferFactor) : 
    ProcessBlock(name, 1, 1), 
    size(size), 
    inverse(inv), 
    fftWindow(win), 
    inBufferFactor(inBufferFactor),
    outBufferFactor(outBufferFactor) 
{

	setInputType(0, typeid(cfloat_t));
	kissState = kiss_fft_alloc(size, inverse, 0, 0);
    setWindow(fftWindow);
    work = 1;
}


FFT::~FFT() {
	kiss_fft_cleanup();
}

int FFT::setSize(int s) {
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

	kiss_fft_cleanup();
	kissState = kiss_fft_alloc(size, inverse, 0, 0);
    setWindow(fftWindow);
	work = 1;
    return size;
}
        
void FFT::setWindow(FFTWindow win) {
    fftWindow = win;
    calcWindow(fftWindow, window, size, inverse);
}

int FFT::process(cfloat_t* in0, int in0Cnt, cfloat_t* out, int outCnt) {


    if (in0Cnt < size)
        return 0;
    if (outCnt == in0Cnt) {
        if (fftWindow == None) {
            kiss_fft(kissState, (kiss_fft_cpx*)in0, (kiss_fft_cpx*)out);
        } else {
			if (inverse) {
				kiss_fft(kissState, (kiss_fft_cpx*)in0, (kiss_fft_cpx*)out);
				volk_32fc_32f_multiply_32fc((cfloat*)out, (cfloat*)out, &window[0], size);
			} else {
				volk_32fc_32f_multiply_32fc((cfloat*)out, (cfloat*)in0, &window[0], size);
				kiss_fft(kissState, (kiss_fft_cpx*)out, (kiss_fft_cpx*)out);
			}
        }
    } else {
        int offset = (in0Cnt - outCnt)/2;
        if (fftWindow == None) {
            kiss_fft(kissState, (kiss_fft_cpx*)in0, (kiss_fft_cpx*)tmpOut);
            memcpy(out, tmpOut + offset, outCnt*sizeof(cfloat_t));
        } else {
            if (inverse) {
                kiss_fft(kissState, (kiss_fft_cpx*)in0, (kiss_fft_cpx*)tmpOut);
#if 1
				for (int i=offset;i<size-offset;i++) {
					tmpOut[i].re *= window[i]; 
					tmpOut[i].im *= window[i]; 
				}
#else
				// crash in volk ??
                // volk_32fc_32f_multiply_32fc((cfloat*)&tmpOut[0], (cfloat*)&tmpOut[0], &window[0], size);
#endif
                memcpy(out, tmpOut + offset, outCnt*sizeof(cfloat_t));
            } else {
                volk_32fc_32f_multiply_32fc((cfloat*)tmpOut, (cfloat*)in0, &window[0], size);
                kiss_fft(kissState, (kiss_fft_cpx*)tmpOut, (kiss_fft_cpx*)tmpOut);
                memcpy(out, tmpOut + offset, outCnt*sizeof(cfloat_t));
            }
        }
    }
    return outCnt;
}


int FFT::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
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

    int outSize = size/outBufferFactor;

    while (inBuf[0]->size() >= (unsigned int)size) {
        auto outBuf = std::make_shared<ProcessBuffer> (outSize, typeid(cfloat_t));

        int nElements = process((cfloat_t*)**(inBuf[0]), size, (cfloat_t*)**(outBuf), outSize);

        inBuf[0]->processed(size/inBufferFactor);
        outBuf->setSize(nElements);

        setGlobTime(name.data());
        send(outBuf, 0, recursion+1);
    }
    return 0;
}
