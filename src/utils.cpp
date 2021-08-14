#include "utils.h"
#include <GLFW/glfw3.h>
std::vector<glm::vec3> myGlTransformVec(std::vector<glm::vec3> v, glm::mat4 m){
  std::vector<glm::vec3> res;
  for (unsigned int ix = 0; ix < v.size(); ++ix)
    res.push_back(glm::vec3(m*glm::vec4(v[ix], 1.0f)));
  return res;
}

std::vector<glm::vec3> scaleXYZ(std::vector<glm::vec3> v, float scaleX, float scaleY, float scaleZ){
  std::vector<glm::vec3> res;
  for (unsigned int ix = 0; ix < v.size(); ++ix)
    res.push_back(glm::vec3(scaleX * v[ix].x, scaleY * v[ix].y, scaleZ * v[ix].z));
  return res;
}

double getTime(){
  return glfwGetTime();
}
