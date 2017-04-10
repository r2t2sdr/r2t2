#ifndef _AGC_H_
#define _AGC_H_

#include "types.h"
#include "ProcessBlock.h"

class AGC : public ProcessBlock 
{
	public: 
		AGC (std::string name) ;
		~AGC ();

		int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion=0);
		void setDec(int);
		void setAtt(int);

	private:
		int process(float* in0, int in0Cnt);
		float att,dec,gain,ref,peek;
};

#endif
