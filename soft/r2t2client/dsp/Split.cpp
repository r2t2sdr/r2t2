#include "Split.h"
#include "types.h"
#include "lib.h"

Split::Split(std::string name) : ProcessBlock(name, 1, 2) {
    setInputType(0, typeid(cfloat_t));
}

Split::~Split() {
}

int Split::receive(std::shared_ptr<ProcessBuffer> buf, uint32_t input, int recursion) {

	if (input != 0)
		return 0;

    auto outBuf = std::make_shared<ProcessBuffer> (buf->getCapacity(), typeid(cfloat_t));

    outBuf->append(buf.get());

	setGlobTime(name.data());
	send(buf, 0, recursion+1);
	send(outBuf, 1, recursion+1);
	return 0;
}
