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
	fft = new FFT("fftWaterfall", 2048, false, BlackmanNuttall, 1);
    split = new Split("split");
    nfSplit = new Split("nfSplit");
    fftInterpol = new FFTInterpol("interpol", 2048);
	agc = new AGC("agc");
    sMeter = new SMeter("smeter");
    notch = new Notch("notch");
    null = new Null("Null");
	fftFilter1 = new FFT_real ("fft1", 1024, Hanning, 2);
	fftFilter2 = new FFTi_real("fft2", 1024, Hanning, 2);
    noiseFilter = new NoiseFilter("noiseFilter");

	ProcessBlock::connect(0, split, 0);
	ProcessBlock::connect(1, fft, 0);

    fft->connect(0, fftInterpol, 0);

    split->connect(0, rotate1, 0);
    split->connect(1, null, 0);
    rotate1->connect(0, firDecim, 0);
    firDecim->connect(0, rotate2, 0); 
	rotate2->connect(0, nfSplit, 0);

	nfSplit->connect(0, ssbDemod, 0);
    ssbDemod->connect(0, agc, 0); 
    // agc->connect(0, g711Encode, 0); 
    
    agc->connect(0, fftFilter1, 0); 
    fftFilter1->connect(0, noiseFilter, 0);
    noiseFilter->connect(0, fftFilter2, 0);
    fftFilter2->connect(0, g711Encode, 0);

	nfSplit->connect(1, sMeter, 0);

    setMode(RX_LSB);
    setFilter(-3000,3000);

    running = true;
}

RX::~RX() {
    mutex.lock();
    running = false;
    mutex.unlock();
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
    delete notch;
    delete null;
    delete fftFilter1;
    delete fftFilter2;
    delete noiseFilter;
}

int RX::setFFTSize(int size) {
    if (size>4096)
        size=4096;
    fftInterpol->setSize(size);
    return fft->setSize(size);
}

void RX::setAGCDec(int n) {
	agc->setDec(n);
}

void RX::setNotch(bool n) {
    return;
    if (n) {
        agc->connect(0, notch, 0);
        notch->connect(0, g711Encode, 0); 
    } else
        agc->connect(0, g711Encode, 0); 
}

void RX::setFFTAudio(bool audio) {
    if (audio) {
        split->connect(1, fft, 0);
        ProcessBlock::connect(1, null, 0);
    } else {
        split->connect(1, null, 0);
        ProcessBlock::connect(1, fft, 0);
    }
}

void RX::setFilter(int32_t low, int32_t high) {
	if (low >= high) {
		qDebug() << "file low >= high: " << low << high << ", ignoring";
        return;
	}
	if (abs(low) >= (int32_t)sampleRate/2)  {
		// qDebug() << "Filter low freq out of range: " << low;
		low = -(int32_t)sampleRate/2+1;
	}
	if (abs(high) >= (int32_t)sampleRate/2)  {
		// qDebug() << "Filter high freq out of range: " << high; 
		high = (int32_t)sampleRate/2-1;
	}

	float cutOffFreq = (float)(abs(high-low))/sampleRate/2;
	if (cutOffFreq < 100.0/sampleRate)
		cutOffFreq = 100.0/sampleRate;
	if (cutOffFreq > 0.5)
		cutOffFreq = 0.5;
    firDecim->setCutOffFreq(cutOffFreq);
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
			break;
        case RX_FM:
			nfSplit->connect(0, fmDemod, 0);
			fmDemod->connect(0, agc, 0); 
			break;
        default:
            ;
    }

}

void RX::setNoise(int v) {
    noiseFilter->setLevel(v);
}

int RX::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
    int n=0;
	startGlobTime();

    mutex.lock();
    if (running) 
        n = inputProcess(buf, input, recursion);
    mutex.unlock();
    return n;
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

