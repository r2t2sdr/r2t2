#include <QDebug>
#include "Null.h"
#include "types.h"
#include "lib.h"


Null::Null(std::string name) : ProcessBlock(name, 1, 0) {
	setInputType(0, typeid(cfloat_t));
}

Null::~Null() {
}



int Null::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {
	int minBuf = preReceive(buf, input, recursion);
	if (minBuf<0)
		return 0;

	setGlobTime(name.data());
	inBuf[input].reset();
	return 0;
}
