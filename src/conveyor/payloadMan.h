#pragma once
#include "srcEngine/API_fwd.h"
#include <vector>
#include <glm/vec3.hpp>
#include <cmath> // std::nanf()
#include <unordered_set>
namespace game {
class payload;
class payloadTrain;
class payloadMan {
public:
	payloadMan(mgeng::root *mgengRoot, std::vector<glm::vec3> colorscheme);
	payload* payload_new();
	void timeslice(float now_s);
protected:
	mgeng::instanced *inst;
	float lastNow_s = std::nanf("");
	std::unordered_set<payloadTrain*> payloadTrains;
};
} // namespace
