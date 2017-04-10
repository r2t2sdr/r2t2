#ifndef _PROCESS_BUFFER_H_
#define _PROCESS_BUFFER_H_

#include <vector>
#include <map>
#include <iostream>
//#include "dsp/blockAllocator.h"

extern int processBufCnt, maxProcessBufCnt;	

class ProcessBuffer {

	public:

		ProcessBuffer(uint32_t size, const std::type_info &type);
		~ProcessBuffer();
		void* operator*();
		uint32_t size();
		uint32_t getCapacity();
		void setSize(uint32_t len);
		uint32_t append(ProcessBuffer* buf);
		uint32_t  processed(uint32_t len);
		int setType(const std::type_info &t);	
		void debug(std::string s, int level=0);
		const char* typeName();
		std::shared_ptr<ProcessBuffer> getPart(int offset, int len);
		void clear();
	private:
		typedef std::map<const std::type_info*, std::size_t> sizes_container; 

		uint32_t getTypeSize();
		template<typename T> void registerSize();
		template <typename T> void debugBuf(T p, int len);

		sizes_container typeSizes;
		
		uint8_t *buf;
		const std::type_info *type;
		uint32_t bufSize;
		uint32_t typeSize;
		uint32_t capacity;
};

#endif // _PROCESS_BUFFER_H_
