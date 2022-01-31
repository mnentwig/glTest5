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
	void ctrlInput(const preDrawState *pds);
	void setPitchYawRollKeys(char pitchDown, char pitchUp, char YawLeft,
			char YawRight, char rollCcw, char rollCw);
	void setMovementKeys(char forwards, char backwards, char left, char right,
			char up, char down);
	const glm::mat4x4& getView() const;

protected:
	engine *e;
	glm::quat viewQuat;
	glm::mat4x4 viewMatrix;
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
	const glm::vec3 vecRight = glm::vec3(1, 0, 0);
	const glm::vec3 vecUp = glm::vec3(0, 1, 0);
	const glm::vec3 vecForw = glm::vec3(0, 0, -1);
};
}
