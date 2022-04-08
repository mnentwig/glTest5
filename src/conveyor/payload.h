#pragma once
#include "convMan.h"
namespace mgeng {
class instanced;
}
namespace game {
class convSeg;
class payloadMan;

/// something that moves on a conveyor
class payload {
public:
	payload(payloadMan *plm);
	void setPosition(convSeg *conveyor, float pos);
	void move(float deltaPos);
	void attachTo(payload *prev);
	void render(const glm::mat4& proj, const std::vector<glm::vec3> *colorscheme);

	//struct positionComparator {
	//	bool operator ()(const payload *lhs, const payload *rhs) {
	//		return lhs->pos <= rhs->pos;
	//	}
	//};
	convSeg *currentConvSeg = NULL;
	float pos = std::nanf("");
protected:
	payloadMan *plm;
	mgeng::instanced *inst = NULL;
	payload *prev = NULL;
	payload *next = NULL;
	friend payloadMan;
};

} // namespace
