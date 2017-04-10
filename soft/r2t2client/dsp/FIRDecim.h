#ifndef _FIRDECIM_H_
#define _FIRDECIM_H_

#include <gnuradio/filter/fir_filter_with_buffer.h>
#include <gnuradio/filter/firdes.h>
#include "types.h"
#include "ProcessBlock.h"

class FIRDecim: public ProcessBlock 
{

	public:
		FIRDecim(std::string name, int decim, float cutOffFreq);
		~FIRDecim();
		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);
        void setCutOffFreq(float f);

	private:
		int process(cfloat_t* in, int cnt, cfloat_t* out);

		int decimation;
        gr::filter::kernel::fir_filter_with_buffer_ccf *firFilter;
        gr::filter::firdes *firdes;
};

#endif
