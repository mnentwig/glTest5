#include "explTraj.h"
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include "API.h"
namespace engine {
explTraj::explTraj() {
	this->accTime_s = 0;
}
void explTraj::registerFragment(glm::vec3 dirNorm, float speed, glm::vec3 rotAxis, float rotAngSpeed_radPerSecond) {
	this->fragDirNorm.push_back(dirNorm);
	this->posOffset.push_back(glm::vec3(0, 0, 0));
	this->speed.push_back(speed);
	this->rotAxis.push_back(rotAxis);
	this->rotAngSpeed_radPerSecond.push_back(rotAngSpeed_radPerSecond);
}

glm::vec3 explTraj::getDeltaPos(unsigned int fragIx) const {
	return this->posOffset[fragIx];
}

glm::mat4 explTraj::getRotation(unsigned int fragIx) const {
	float phi = this->rotAngSpeed_radPerSecond[fragIx] * this->accTime_s;
	return glm::rotate(glm::mat4(1.0f), phi, this->rotAxis[fragIx]);
}

void explTraj::clear() {
	this->fragDirNorm.clear();
	this->posOffset.clear();
	this->speed.clear();
	this->rotAxis.clear();
	this->rotAngSpeed_radPerSecond.clear();
	this->accTime_s = 0;
}

void explTraj::clock(float deltaT_s) {
	this->accTime_s += deltaT_s;
	for (unsigned int ix = 0; ix < this->fragDirNorm.size(); ++ix) {
		this->posOffset[ix] += this->fragDirNorm[ix] * deltaT_s * this->speed[ix];
	}
}
} // namespace

//TODO add const below to params
mgeng::instancedExplosion::instancedExplosion(mgeng::root *root) {
	this->traj = new engine::explTraj();
}

void mgeng::instancedExplosion::clock(float deltaT_s) {
	this->traj->clock(deltaT_s);
}
