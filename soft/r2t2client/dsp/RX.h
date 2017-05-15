#ifndef _RX_H_
#define _RX_H_

#include <QMutex>
#include <stdint.h>
#include "ProcessBlock.h"
#include "AMDemod.h"
#include "SSBDemod.h"
#include "FMDemod.h"
#include "G711Encode.h"
#include "FIRDecim.h"
#include "Rotate.h"
#include "FFT.h"
#include "FFT_real.h"
#include "FFTi_real.h"
#include "FFTInterpol.h"
#include "Split.h"
#include "AGC.h"
#include "SMeter.h"
#include "Notch.h"
#include "NoiseFilter.h"
#include "Null.h"

enum RXMode {
    RX_LSB,
    RX_USB,
    RX_DSB,
    RX_CWL,
    RX_CWU,
    RX_FM,
    RX_AM,
    RX_DIGU,
    RX_SPEC,
    RX_DIGL,
    RX_SAM,
    RX_DRM
}; 

class RX: public ProcessBlock 
{

	public:
		RX(std::string name, uint32_t sampleRate);
		~RX();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);
		int connect(uint32_t output, ProcessBlock *dest, uint32_t input);
        int setFFTSize(int size);
		void setAGCDec(int);
        void setMode(RXMode);
        void setFilter(int32_t lo, int32_t high);
        void setNotch(bool);
        void setFFTAudio(bool);
        void setNoise(int);

	private:

		AMDemod *amDemod; 
		SSBDemod *ssbDemod; 
		FMDemod *fmDemod; 
        G711Encode *g711Encode;
		FIRDecim *firDecim;
		Rotate *rotate1, *rotate2;
        FFT *fft;
        FFT_real *fftFilter1; 
        FFTi_real *fftFilter2;
        FFTInterpol *fftInterpol;
        Split *split, *nfSplit;
		AGC *agc;
        SMeter *sMeter;
        Notch *notch;
        NoiseFilter *noiseFilter;
        Null *null;

        QMutex mutex;
        RXMode rxMode;
        int32_t rxOffset;
        uint32_t sampleRate;
        uint32_t decimation;
        bool running;
};

#endif
