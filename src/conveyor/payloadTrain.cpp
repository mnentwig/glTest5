#include "payloadTrain.h"
namespace game {

payloadTrain::payloadTrain() {
}

void payloadTrain::timeslice(float deltaT_s) {
	for (auto pl : this->payloads) {
//		pl->timeslice(deltaT_s);
	}
}

payload* payloadTrain::getHead() const {
	auto it = this->payloads.begin();
	if (it == this->payloads.end())
		return NULL;
	return *it;
}

payload* payloadTrain::getTail() const {
	auto it = this->payloads.end();
	if (it == this->payloads.begin())
		return NULL;
	return *(it--);
}

} // namespace
