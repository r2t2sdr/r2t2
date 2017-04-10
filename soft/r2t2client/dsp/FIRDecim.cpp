#include <QDebug>
#include <string.h>
#include "FIRDecim.h"
#include "types.h"
#include "lib.h"
#include "config.h"

FIRDecim::FIRDecim(std::string name, int dec, float cutOffFreq) : ProcessBlock(name, 1, 1), decimation(dec) {
    setInputType(0, typeid(cfloat_t));
    firdes = new gr::filter::firdes();
    firFilter = new gr::filter::kernel::fir_filter_with_buffer_ccf(
            firdes->low_pass(1, 1.0, 1.0/decimation*cutOffFreq, 0.2/decimation*cutOffFreq ) );
}

FIRDecim::~FIRDecim() {
	delete firdes;
	delete firFilter;
}

int FIRDecim::process(cfloat_t* in, int cnt, cfloat_t* out) {
    firFilter->filterNdec((gr_complex*)out, (gr_complex*)in, cnt/decimation, decimation); 
    return cnt/decimation;
}

void FIRDecim::setCutOffFreq(float f) {
    //qDebug() << f;
    firFilter->set_taps(firdes->low_pass(1, 1.0, f, 0.3*f));
    //qDebug() << "filter len:" <<  firdes->low_pass(1, 1.0, f, 0.3*f).size() << f << 1.0/f;
}

int FIRDecim::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
    int minBuf = preReceive(buf, input, recursion);
    if (minBuf<0)
        return 0;

    if (inBuf[0]->size() < 1024) 
        return 0;

    auto outBuf = std::make_shared<ProcessBuffer> (DSP_BUFFER_SIZE, typeid(cfloat_t));

	setGlobTime(name.data());
    int nElements = process((cfloat_t*)**(inBuf[0]), inBuf[0]->size(), (cfloat_t*)**(outBuf));

	inBuf[0]->processed(nElements*decimation);
	outBuf->setSize(nElements);
	setGlobTime(name.data());
	send(outBuf, 0, recursion+1);
	return 0;
}
