#pragma once
#include <vector>
#include <glm/fwd.hpp> // Eclipse CODAN needs this
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
namespace engine {
class explTraj {
public:
	explTraj();
	explTraj(const explTraj&) = delete; // prevent copy
	explTraj& operator =(const explTraj&) = delete; // prevent copy

	void registerFragment(glm::vec3 dirNorm, float speed, glm::vec3 rotAxis, float rotAngSpeed_radPerSecond);
	glm::vec3 getDeltaPos(unsigned int fragIx) const;
	glm::mat4 getRotation(unsigned int fragIx) const;
	void clock(float deltaT_s);
	void clear();
protected:
	std::vector<glm::vec3> fragDirNorm;
	std::vector<glm::vec3> posOffset;
	std::vector<float> speed;
	std::vector<glm::vec3> rotAxis;
	std::vector<float> rotAngSpeed_radPerSecond;

	float accTime_s;
};
}
