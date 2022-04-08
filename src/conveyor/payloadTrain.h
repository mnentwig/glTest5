#pragma once
#include <cmath>
#include <list>
namespace game {
class convSeg;
class payload;
class payloadTrain {
public:
	payloadTrain();
	void timeslice(float deltaT_s);
	std::list<payload*> payloads;
	payload* getHead() const;
	payload* getTail() const;
protected:
	float speed = std::nanf("");
};
} // namespace
