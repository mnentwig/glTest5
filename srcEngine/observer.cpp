#include "observer.h"
#include "glmPrint.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/quaternion_trigonometric.hpp>
#include <iostream> // !!!!!
namespace engine {

observer::observer(engine *e) {
	this->e = e;
	float defrate_radps = 100 * 3.141592 / 180.0;
	this->dPitchDt_radps = defrate_radps;
	this->dYawDt_radps = defrate_radps;
	this->dRollDt_radps = defrate_radps;

	glm::vec3 eye(0, 5, 0);
    glm::vec3 far(0, 5, -1);
    glm::vec3 up(0, 1, 0);

	this->viewMatrix = glm::lookAt (eye, far, up);
	this->viewQuat = glm::quat_cast(this->viewMatrix);
}

const glm::mat4x4& observer::getView() const {
	return this->viewMatrix;
}

void observer::setPitchYawRollKeys(int pitchDown, int pitchUp, int yawLeft,
		int yawRight, int rollCcw, int rollCw) {
	this->keycodePitchDown = pitchDown;
	this->keycodePitchUp = pitchUp;
	this->keycodeYawLeft = yawLeft;
	this->keycodeYawRight = yawRight;
	this->keycodeRollCcw = rollCcw;
	this->keycodeRollCw = rollCw;
}

void observer::setMovementKeys(int forwards, int backwards, int left,
		int right, int up, int down) {
	this->keycodeForwards = forwards;
	this->keycodeBackwards = backwards;
	this->keycodeLeft = left;
	this->keycodeRight = right;
	this->keycodeUp = up;
	this->keycodeDown = down;
}

void observer::ctrlInput(const preDrawState *pds) {
	float dPitch = 0;
	float dYaw = 0;
	float dRoll = 0;
	// positive angle turns CCW
	dPitch += this->e->testKeycodePressed(this->keycodePitchDown) ? 1 : 0;
	dPitch += this->e->testKeycodePressed(this->keycodePitchUp) ? -1 : 0;
	dYaw += this->e->testKeycodePressed(this->keycodeYawRight) ? 1 : 0;
	dYaw += this->e->testKeycodePressed(this->keycodeYawLeft) ? -1 : 0;
	dRoll += this->e->testKeycodePressed(this->keycodeRollCcw) ? 1 : 0;
	dRoll += this->e->testKeycodePressed(this->keycodeRollCw) ? -1 : 0;
	dPitch *= this->dPitchDt_radps*pds->deltaTime_s;
	dYaw *= this->dYawDt_radps*pds->deltaTime_s;
	dRoll *= this->dRollDt_radps*pds->deltaTime_s;

	glm::vec3 axis(0);
	axis += dPitch * this->vecRight;
	axis += dYaw*this->vecUp;
	axis += dRoll*this->vecForw;
	float len = glm::length(axis);
	if (len > 1e-7f){
		axis *= 1.0f/len;
		glm::quat deltaRot = angleAxis(/*angle*/len, axis);
		this->viewQuat = deltaRot * this->viewQuat;
	}

	float deltaX = 0;
	deltaX += this->e->testKeycodePressed(this->keycodeRight) ? 1 : 0;
	deltaX += this->e->testKeycodePressed(this->keycodeLeft) ? -1 : 0;
	float deltaY = 0;
	deltaY += this->e->testKeycodePressed(this->keycodeUp) ? 1 : 0;
	deltaY += this->e->testKeycodePressed(this->keycodeDown) ? -1 : 0;
	float deltaZ = 0;
	deltaZ += this->e->testKeycodePressed(this->keycodeForwards) ? -1 : 0;
	deltaZ += this->e->testKeycodePressed(this->keycodeBackwards) ? 1 : 0;

	glm::vec3 delta(deltaX, deltaY, deltaZ);

	glm::vec3 eye(0, 5, 0);
//	this->viewMatrix = glm::translate(glm::mat4(), -eye) * glm::mat4_cast(this->viewQuat);
	glmPrint(this->viewMatrix);
	glmPrint(glm::mat3_cast(this->viewQuat));
	glmPrint(glm::translate(glm::mat4_cast(this->viewQuat), -eye));

	this->pos += delta * this->viewQuat;

	this->viewMatrix = glm::translate(glm::mat4_cast(this->viewQuat), -this->pos);

}
} // namespace
