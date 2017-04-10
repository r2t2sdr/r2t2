#include <QDebug>
#include "G711Encode.h"
#include "types.h"
#include "lib.h"
#include "g711.h"


G711Encode::G711Encode(std::string name) : ProcessBlock(name, 1, 1) {
    setInputType(0, typeid(float));
}

G711Encode::~G711Encode() {
}


int G711Encode::process(float* in, int cnt, int8_t* out) {
	int processed = 0;
	while (processed < cnt) {
        *out++ = linear2alaw( (int)((*in++)*0x8000));
		processed++;
	}
	return processed;
}

int G711Encode::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	int minBuf = preReceive(buf, input, recursion);
	if (minBuf<0)
		return 0;

	int nElements = process((float*)**(inBuf[0]), inBuf[0]->size(), (int8_t*)**(inBuf[0]));

	inBuf[0]->setType(typeid(int8_t));
	inBuf[0]->setSize(nElements);

	setGlobTime(name.data());
	send(inBuf[0], 0, recursion+1);
	inBuf[0].reset();
	return 0;
}
