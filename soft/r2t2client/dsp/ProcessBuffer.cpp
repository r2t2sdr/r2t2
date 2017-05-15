#include <memory>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <stdexcept>
#include <assert.h>
#include <string.h>
#include "lib.h"
#include "dsp/ProcessBuffer.h"
#include "types.h"
#include "config.h"

// #define DEBUG

int processBufCnt=0;	
int maxProcessBufCnt=0;	

ProcessBuffer::ProcessBuffer(uint32_t size, const std::type_info &t):type(&t) {

	registerSize<uint32_t>();
	registerSize<int32_t>();
	registerSize<uint16_t>();
	registerSize<int16_t>();
	registerSize<uint8_t>();
	registerSize<int8_t>();
	registerSize<float>();
	registerSize<double>();
	registerSize<int>();
	registerSize<cfloat_t>();

	typeSize = getTypeSize();
	capacity = size*typeSize;
	buf = (uint8_t*)malloc(capacity);
	bufSize = 0;

	processBufCnt++;
	if (processBufCnt > maxProcessBufCnt)
		maxProcessBufCnt = processBufCnt;
}

ProcessBuffer::~ProcessBuffer() {
	free(buf);
	processBufCnt--;
}

void* ProcessBuffer::operator *() {
	return buf;
}

uint32_t ProcessBuffer::getTypeSize() {
	int typeSize;
	auto it = typeSizes.find(type);
	if (it != typeSizes.end())
		typeSize =  it->second;
	else  {
		assert(0); // type not registered
		typeSize = 1;
	}
	return typeSize;
}

int ProcessBuffer::setType(const std::type_info &t) {
	type = &t;
	typeSize = getTypeSize();
	return typeSize;
}

uint32_t ProcessBuffer::append(ProcessBuffer* ibuf) {
	if (size() + ibuf->size() > getCapacity()) {
		printf ("%i %i %i\n",(int)size(), ibuf->size(), getCapacity());
		assert(0);
		return size();
	}
	memcpy (buf + bufSize, **ibuf, ibuf->size()*typeSize);
	bufSize += ibuf->size()*typeSize;
	return size();
}

void ProcessBuffer::clear() {
	memset(buf, 0, bufSize);
}

uint32_t ProcessBuffer::processed(uint32_t len) {
    uint32_t l = len*typeSize;
    if (len > bufSize) {
        assert(0);
        bufSize = 0;
        return 0;
    }
    if (capacity < l) {
        assert(0);
        bufSize = 0;
        return 0;
    }

    memmove(buf, buf+l, capacity-l);
    bufSize -= l;
    return size();
}

uint32_t ProcessBuffer::size() {
	return bufSize/typeSize;
}

void ProcessBuffer::setSize(uint32_t len) {
	assert(getCapacity() >= len);
	bufSize = len*typeSize;
}

uint32_t ProcessBuffer::getCapacity() {
	return capacity/typeSize;
}

#ifdef DEBUG
void ProcessBuffer::debug(std::string s, int level) {
	int maxLen = std::min((int)(bufSize/typeSize), 16 );
	void *p = buf;
	std::string format = std::string("                         ").substr(0, level*2) + std::string(" %s (%i/%i) %s(%i)");

	PDEBUG(MSG1, format.c_str(), s.c_str(), size(), getCapacity(), type->name(), typeSize);

	if (type->name() == typeid(int32_t).name()) {
		debugBuf((int32_t*)p, maxLen);
	} else if (type->name() == typeid(uint32_t).name()) {
		debugBuf((uint32_t*)p, maxLen);	
	} else if (type->name() == typeid(int16_t).name()) {
		debugBuf((int16_t*)p, maxLen);	
	} else if (type->name() == typeid(uint16_t).name()) {
		debugBuf((uint16_t*)p, maxLen);	
	} else if (type->name() == typeid(float).name()) {
		debugBuf((float*)p, maxLen);	
	}
}
#else
void ProcessBuffer::debug(std::string, int) {}
#endif

template <typename T> void ProcessBuffer::debugBuf(T p, int len) {
#ifdef DEBUG
	std::stringstream ss;
	for (int i=0;i<len;i++) {
		ss << p[i] << " ";
	}
	ss << "\n";
#endif
}

template<typename T> void ProcessBuffer::registerSize() {
	typeSizes.insert(sizes_container::value_type(&typeid(T), sizeof(T)));
}

const char* ProcessBuffer::typeName() {
	return type->name();
}
		
std::shared_ptr<ProcessBuffer> ProcessBuffer::getPart(int offset, int len) {
	if (len+offset > (int)this->size()) {
		assert(0);
		len = 0; 
	}
	auto obuf = std::make_shared<ProcessBuffer> (DSP_BUFFER_SIZE, *type);
	obuf->setSize(len);

	uint8_t* to = (uint8_t*)**obuf;
	uint8_t* from = (uint8_t*)**this;
	from += offset*typeSize;
	memcpy(to, from, len*typeSize);

	return obuf;
}
