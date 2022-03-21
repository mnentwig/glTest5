#include "convSeg.h"
#include "srcEngine/API.h"
#include <cassert>
namespace game {
convSeg::convSeg() {
}

void convSeg::render(const glm::mat4x4 proj, const std::vector<glm::vec3> *colorscheme) {
	glm::mat4 projT = proj * this->model2world;
	this->inst->render(projT, *colorscheme);
	for (auto it : this->payloads) {
		it->render(proj, colorscheme);
	}
}

void convSeg::registerPayload(payload *pl) {
	assert(this->payloads.find(pl) == this->payloads.end());
	this->payloads.emplace(pl);

}

void convSeg::unregisterPayload(payload *pl) {
	assert(this->payloads.find(pl) != this->payloads.end());
	this->payloads.erase(pl);
}

void convSeg::setStartEndPt(const glm::vec3 &startPt, const glm::vec3 &endPt) {
	this->startPtModel = startPt;
	this->endPtModel = endPt;
	this->length = glm::length(this->endPtModel - this->startPtModel);

	this->initState = GOT_ENDPOINTS;
}

void convSeg::setModel2world(const glm::mat4 &model2world) {
	assert(this->initState >= GOT_ENDPOINTS);
	this->model2world = model2world;
	this->startPtWorld = model2world * glm::vec4(this->startPtModel, 1.0f);
	this->endPtWorld = model2world * glm::vec4(this->endPtModel, 1.0f);
	this->initState = GOT_MATRIX;
}

glm::vec3 convSeg::getPayloadPos(float pos){
	assert(this->initState == GOT_MATRIX);
	assert(pos >= 0);
	assert(pos <= this->length);
	return glm::mix(this->startPtWorld, this->endPtWorld, pos / this->length);
}

} // namespace
