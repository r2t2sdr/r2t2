#include <QDebug>
#include <string.h>
#include <volk/volk.h>
#include "FFTInterpol.h"
#include "types.h"
#include "lib.h"
#include "config.h"

FFTInterpol::FFTInterpol(std::string name, int size) : ProcessBlock(name, 1, 1), size(size) {
	setInputType(0, typeid(cfloat_t));
}


FFTInterpol::~FFTInterpol() {
}


int FFTInterpol::process(cfloat_t* in0, int in0Cnt) {
	
    float step = (float)in0Cnt/size;
    float pos = 0;
	int ip;
    float scale = 1.0/(in0Cnt*in0Cnt);
    for (int i=0;i<size;i++) {
		ip = (int)pos;
		if (ip<in0Cnt/2)
			ip = in0Cnt/2+ip;
		else
			ip = ip-in0Cnt/2;

        float x = ((float*)in0)[ip*2];
        float y = ((float*)in0)[ip*2+1];
        float q = x*x+y*y;
        if (q==0)
            q=1e-10;

        float ampl = -log10(q*scale)*10 - amplFFTOffset;
        ampl = checkRange(ampl, 0, 255);

        sBuffer[i]=ampl;
        pos+=step;
    }
    memcpy (in0, sBuffer, size);
	return size;
}

void FFTInterpol::setSize(int s) {
	size = s;
}

int FFTInterpol::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	int minBuf = preReceive(buf, input, recursion);
	if (minBuf<0)
		return 0;

    int nElements = process((cfloat_t*)**(inBuf[0]), inBuf[0]->size());

	inBuf[0]->setType(typeid(uint8_t));
	inBuf[0]->setSize(nElements);

	setGlobTime(name.data());
	send(inBuf[0], 0, recursion+1);
	inBuf[0].reset();
	return 0;
}
