#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
class explTraj{
 public:
  explTraj();
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
