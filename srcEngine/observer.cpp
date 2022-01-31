#include "observer.h"
#include "glmPrint.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/quaternion_trigonometric.hpp>

namespace engine {

observer::observer(engine *e) {
	this->e = e;
	float defrate_radps = 0.3 * 3.141592 / 180.0;
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

void observer::setPitchYawRollKeys(char pitchDown, char pitchUp, char yawLeft,
		char yawRight, char rollCcw, char rollCw) {
	this->keycodePitchDown = pitchDown;
	int keycodePitchUp = pitchUp;
	int keycodeYawLeft = yawLeft;
	int keycodeYawRight = yawRight;
	int keycodeRollCcw = rollCcw;
	int keycodeRollCw = rollCw;
}

void observer::setMovementKeys(char forwards, char backwards, char left,
		char right, char up, char down) {
	int keycodeForwards = forwards;
	int keycodeBackwards = backwards;
	int keycodeLeft = left;
	int keycodeRight = right;
	int keycodeUp = up;
	int keycodeDown = down;
}

void observer::ctrlInput(const preDrawState *pds) {
	float dPitch = 0;
	float dYaw = 0;
	float dRoll = 0;
	dPitch += this->e->testKeycodePressed(this->keycodePitchDown) ? -1 : 0;
	dPitch += this->e->testKeycodePressed(this->keycodePitchUp) ? 1 : 0;
	dYaw += this->e->testKeycodePressed(this->keycodeYawRight) ? -1 : 0;
	dYaw += this->e->testKeycodePressed(this->keycodeYawLeft) ? 1 : 0;
	dRoll += this->e->testKeycodePressed(this->keycodeRollCcw) ? -1 : 0;
	dRoll += this->e->testKeycodePressed(this->keycodeRollCw) ? 1 : 0;
	dPitch *= this->dPitchDt_radps;
	dYaw *= this->dYawDt_radps;
	dRoll *= this->dRollDt_radps;

	glm::vec3 axis;
	axis += dPitch * this->vecRight;
	axis += dYaw*this->vecUp;
	axis += dRoll*this->vecForw;
	float len = glm::length(axis);
	if (len > 1e-7f){
		axis *= 1.0f/len;
		glm::quat deltaRot = angleAxis(/*angle*/len, axis);
		this->viewQuat = this->viewQuat * deltaRot;
	}

//	glm::vec3 eye(0, 5, 0);
//	this->viewMatrix = glm::translate(glm::mat4(), -eye) * glm::mat4_cast(this->viewQuat);
//	glmPrint(glm::mat4(1));
//	exit(1);
}
} // namespace
