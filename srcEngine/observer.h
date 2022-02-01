#pragma once
#include <glm/fwd.hpp> // Eclipse CODAN needs this
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "engine.h"

namespace engine {
class observer {
public:
	observer(engine *e);
    observer(const observer&) = delete; // prevent copy
    observer& operator =(const observer&) = delete; // prevent copy

	void ctrlInput(const preDrawState *pds);
	void setPitchYawRollKeys(int pitchDown, int pitchUp, int YawLeft,
			int YawRight, int rollCcw, int rollCw);
	void setMovementKeys(int forwards, int backwards, int left, int right,
			int up, int down);
	const glm::mat4x4& getView() const;

protected:
	engine *e;
	glm::quat viewQuat;
	glm::mat4x4 viewMatrix;
	glm::vec3 pos = glm::vec3(0, 0, 0);
	int keycodePitchDown = 0;
	int keycodePitchUp = 0;
	int keycodeYawLeft = 0;
	int keycodeYawRight= 0;
	int keycodeRollCcw = 0;
	int keycodeRollCw = 0;
	int keycodeForwards = 0;
	int keycodeBackwards = 0;
	int keycodeLeft = 0;
	int keycodeRight = 0;
	int keycodeUp = 0;
	int keycodeDown = 0;
	float dPitchDt_radps;
	float dYawDt_radps;
	float dRollDt_radps;
	float movementSpeed;
	float mouseSens=0.1;
	const glm::vec3 vecRight = glm::vec3(1, 0, 0);
	const glm::vec3 vecUp = glm::vec3(0, 1, 0);
	const glm::vec3 vecForw = glm::vec3(0, 0, -1);
};
}
