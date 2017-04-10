#ifndef _PROCESS_BLOCK_H_
#define _PROCESS_BLOCK_H_

#include <memory>
//#include <assert.h>
//#include <string.h>
#include <stdint.h>
#include "dsp/ProcessBuffer.h"

class ProcessBlock;

constexpr uint32_t MAX_PIN=4;

class ProcessBlock {

	public: 
		ProcessBlock(std::string name, uint32_t inputs, uint32_t outputs);
		virtual ~ProcessBlock();

		virtual int connect(uint32_t output, ProcessBlock *dest, uint32_t input);
		int disconnect(uint32_t output);
		std::string getName();
		virtual int receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion=0)=0;
	protected:
		int send(std::shared_ptr<ProcessBuffer> buf, uint32_t output, int recursion=0);
		int inputProcess(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion=0);
		int preReceive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion);
		void setInputType(int input, const std::type_info &type); 


		const uint32_t nIn, nOut;
		std::shared_ptr<ProcessBuffer> inBuf[MAX_PIN];
		std::string name;
	private:
		struct next_t { 
			ProcessBlock *dest;
			int32_t input;
		};

		next_t next[MAX_PIN];
		const std::type_info *inType[MAX_PIN];

		int append(std::shared_ptr<ProcessBuffer> buf, uint32_t input);
		int getShortestInput();
};


#endif
