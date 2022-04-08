#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include "srcEngine/API_fwd.h"
#include "srcEngine/util/noncopyable.h"
#include "payload.h"
#include "payloadTrain.h"
#include <set>
#include "convSegArbiter.h"
namespace game {
class convSeg: private mgeng::noncopyable {
public:
	std::vector<convSeg*> prev;
	std::vector<convSeg*> next;
	mgeng::instanced *inst = NULL;
	convSeg();
	void setStartEndPt(const glm::vec3 &startPt, const glm::vec3 &endPt);
	void setModel2world(const glm::mat4 &model2world);
	void render(const glm::mat4x4 proj, const std::vector<glm::vec3> *colorscheme);
	glm::vec3 getPayloadPos(float pos);
	payloadTrain* getAppendablePayloadTrain() const;
	convSegArbiter csa;
protected:
	void registerPayload(payload *pl);
	void unregisterPayload(payload *pl);
	void registerPayloadTrain(payloadTrain *plt);
	void unregisterPayloadTrain(payloadTrain *plt);

	glm::mat4 model2world;
	glm::vec3 startPtModel;
	glm::vec3 endPtModel;
	glm::vec3 startPtWorld;
	glm::vec3 endPtWorld;
	float length = std::nanf("");
//	std::set<payload*, payload::positionComparator> payloads;

	std::set<payload*> payloads;

	/// all payload trains touching this convSeg (start in convSeg and / or end in convSeg or overlap convSeg)
	std::set<payloadTrain*> payloadTrains;
	friend convMan;
	enum initState_e {
		UNINITIALIZED, GOT_ENDPOINTS, GOT_MATRIX
	};
	initState_e initState = UNINITIALIZED;
};

class convSeg_ifToPayloadTrain: protected convSeg {
public:
	void registerPayload(payload *pl);
	void unregisterPayload(payload *pl);
	void registerPayloadTrain(payloadTrain *plt);
	void unregisterPayloadTrain(payloadTrain *plt);
};
} // namespace
