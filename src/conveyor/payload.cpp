#include "payload.h"
#include "convSeg.h"
#include "srcEngine/API.h"
#include <cassert>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>
#include <iostream>
#include "srcEngine/glmPrint.hpp"
namespace game {
payload::payload(payloadMan *plm) {
	this->plm = plm;
}
void payload::setPosition(convSeg *conveyor, float pos) {
	assert(this->currentConvSeg == NULL);
	this->currentConvSeg = conveyor;
	this->pos = pos;
	conveyor->registerPayload(this);
}

void payload::render(const glm::mat4 &proj, const std::vector<glm::vec3> *colorscheme) {
	glm::vec3 pos3d = this->currentConvSeg->getPayloadPos(this->pos);
	glm::mat4 model2world = glm::translate(glm::mat4(1.0f), pos3d);
	glm::mat4 projT = proj * model2world;
glmPrint(model2world);
	this->inst->render(projT, *colorscheme);
	glmPrint(pos3d);
}

} // namespace
