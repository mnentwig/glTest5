#include "convSegArbiter.h"
#include "convSeg.h"
namespace game {
convSegArbiter::convSegArbiter(convSeg *s) {
	this->arbitedConvSeg = s;
}

void convSegArbiter::lockInputPort() {
	this->requestedInputPort = -1;
}

void convSegArbiter::requestUnlockInputPort(int portnum) {
	this->requestedInputPort = portnum;
}

bool convSegArbiter::isInputPortUnlocked(int portnum, float reqFreeDist) {
	if (this->requestedInputPort != portnum)
		return false;
	return this->arbitedConvSeg->checkFreeDistAtInput(reqFreeDist);
}

} // namespace
