#include "payloadMan.h"
#include "srcEngine/API.h"
#include "payload.h"
namespace game {
payloadMan::payloadMan(mgeng::root *mgengRoot, std::vector<glm::vec3> colorscheme) {
	this->inst = new mgeng::instanced(mgengRoot, /*nCols*/colorscheme.size(), /*isOverlay*/false);

	const float r = 0.8;
	const float width = 0.02;
	const unsigned int colIxOutline = 0;
	const unsigned int colIxFill = 1;
	bool hitscanEnable = true;

	glm::vec3 p1 = glm::vec3(r * cos(0.0 / 3.0 * 2.0 * M_PI), 0, r * sin(0.0 / 3.0 * 2.0 * M_PI));
	glm::vec3 p2 = glm::vec3(r * cos(1.0 / 3.0 * 2.0 * M_PI), 0, r * sin(1.0 / 3.0 * 2.0 * M_PI));
	glm::vec3 p3 = glm::vec3(r * cos(2.0 / 3.0 * 2.0 * M_PI), 0, r * sin(2.0 / 3.0 * 2.0 * M_PI));
	glm::vec3 p4 = glm::vec3(0, r, 0);

	std::vector<glm::vec3> v1;
	v1.push_back(p1);
	v1.push_back(p2);
	v1.push_back(p3);
	this->inst->generateOutlinedShape(v1, width, colIxOutline, colIxFill, hitscanEnable);

	std::vector<glm::vec3> v2;
	v2.push_back(p1);
	v2.push_back(p2);
	v2.push_back(p4);
	this->inst->generateOutlinedShape(v2, width, colIxOutline, colIxFill, hitscanEnable);

	std::vector<glm::vec3> v3;
	v3.push_back(p2);
	v3.push_back(p3);
	v3.push_back(p4);
	this->inst->generateOutlinedShape(v3, width, colIxOutline, colIxFill, hitscanEnable);

	std::vector<glm::vec3> v4;
	v4.push_back(p3);
	v4.push_back(p1);
	v4.push_back(p4);
	this->inst->generateOutlinedShape(v4, width, colIxOutline, colIxFill, hitscanEnable);

	this->inst->finalize();
}

payload* payloadMan::payload_new() {
	payload *pl = new payload(this);
	pl->inst = this->inst;
	return pl;
}
}
