#pragma once
#include <cmath> // std::nanf
#include "srcEngine/util/noncopyable.h"

namespace game {
class convSeg;
class convSegArbiter {
public:
	convSegArbiter(convSeg* s);
	void lockInputPort();
	void requestUnlockInputPort(int portnum);
	bool isInputPortUnlocked(int portnum, float reqFreeDist);
protected:
	convSeg* arbitedConvSeg;
	int requestedInputPort = -1;
};
} // namespace
