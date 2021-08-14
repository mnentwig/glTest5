#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

std::vector<glm::vec3> myGlTransformVec(std::vector<glm::vec3> v, glm::mat4 m);
std::vector<glm::vec3> scaleXYZ(std::vector<glm::vec3> v, float scaleX, float scaleY, float scaleZ);
double getTime();
