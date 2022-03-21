#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include "srcEngine/API_fwd.h"
#include "payload.h"
#include <set>
namespace game {
class convSeg {
public:
	std::vector<convSeg*> prev;
	std::vector<convSeg*> next;
	mgeng::instanced *inst = NULL;
	convSeg();
	void setStartEndPt(const glm::vec3 &startPt, const glm::vec3 &endPt);
	void setModel2world(const glm::mat4 &model2world);
	void render(const glm::mat4x4 proj, const std::vector<glm::vec3> *colorscheme);
	void registerPayload(payload *pl);
	void unregisterPayload(payload *pl);
	glm::vec3 getPayloadPos(float pos);
protected:
	glm::mat4 model2world;
	glm::vec3 startPtModel;
	glm::vec3 endPtModel;
	glm::vec3 startPtWorld;
	glm::vec3 endPtWorld;
	float length = std::nanf("");
	std::set<payload*, payload::positionComparator> payloads;
	friend convMan;
	enum initState_e {
		UNINITIALIZED, GOT_ENDPOINTS, GOT_MATRIX
	};
	initState_e initState = UNINITIALIZED;
};
} // namespace
