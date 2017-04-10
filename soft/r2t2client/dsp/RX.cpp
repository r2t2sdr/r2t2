#include <QDebug>
#include <assert.h>
#include "RX.h"
#include "lib.h"

RX::RX(std::string name, uint32_t sampleRate) : ProcessBlock(name, 2, 2), sampleRate(sampleRate) {

    decimation = sampleRate/AUDIO_RATE;

	amDemod = new AMDemod("am_demod");
	ssbDemod = new SSBDemod("ssb_demod");
	fmDemod = new FMDemod("fm_demod");
    g711Encode = new G711Encode("g711encode");
	firDecim = new FIRDecim("fir_decim", decimation, 0.25);
	rotate1 = new Rotate("rotate1", 0);
	rotate2 = new Rotate("rotate2", 0);
	fft = new FFT("fft", 2048);
    split = new Split("split");
    nfSplit = new Split("nfSplit");
    fftInterpol = new FFTInterpol("interpol", 2048);
	agc = new AGC("agc");
    sMeter = new SMeter("smeter");

	ProcessBlock::connect(0, rotate1, 0);
	ProcessBlock::connect(1, fft, 0);

    fft->connect(0, fftInterpol, 0);

    rotate1->connect(0, firDecim, 0);
    firDecim->connect(0, rotate2, 0); 
	rotate2->connect(0, nfSplit, 0);
	nfSplit->connect(0, ssbDemod, 0);
    ssbDemod->connect(0, agc, 0); 
    agc->connect(0, g711Encode, 0); 

	nfSplit->connect(1, sMeter, 0);

    setMode(RX_LSB);
    setFilter(-3000,3000);
}

RX::~RX() {
	delete amDemod;
	delete ssbDemod;
	delete g711Encode;
	delete firDecim;
	delete rotate1;
	delete rotate2;
	delete fft;
	delete split;
	delete nfSplit;
	delete fftInterpol;
	delete agc;
    delete sMeter;
}

int RX::setFFTSize(int size) {
    fftInterpol->setSize(size);
    return fft->setSize(size);
}

void RX::setAGCDec(int n) {
	agc->setDec(n);
}

void RX::setFilter(int32_t low, int32_t high) {
    firDecim->setCutOffFreq((float)(abs(high-low))/sampleRate/2);
    rxOffset = (high+low)/2;
	rotate1->setFreq(-1.0*rxOffset/sampleRate);
    if (rxMode==RX_CWL || rxMode == RX_CWU)
		rotate2->setFreq(1.0*decimation*(rxOffset+CW_OFFSET)/sampleRate);
	else
		rotate2->setFreq(1.0*decimation*rxOffset/sampleRate);
}

void RX::setMode(RXMode mode) {
    rxMode = mode;
    switch (mode) {
        case RX_CWL:
        case RX_CWU:
        case RX_LSB:
        case RX_USB:
        case RX_DSB:
			nfSplit->connect(0, ssbDemod, 0);
			ssbDemod->connect(0, agc, 0); 
            break;
        case RX_AM:
			nfSplit->connect(0, amDemod, 0);
			amDemod->connect(0, agc, 0); 
        case RX_FM:
			nfSplit->connect(0, fmDemod, 0);
			fmDemod->connect(0, agc, 0); 
        default:
            ;
    }

}

int RX::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	startGlobTime();
	return inputProcess(buf, input, recursion);
}

int RX::connect(uint32_t output, ProcessBlock *dest, uint32_t input) {
    if (output == 0)
        return g711Encode->connect(0, dest, input);
    if (output == 1) {
        return fftInterpol->connect(0, dest, input);
    }
    if (output == 2)
        return sMeter->connect(0, dest, input);
    return 0;
}

