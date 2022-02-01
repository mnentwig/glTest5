#pragma once
#include <string>
#include <vector>
#include <glm/fwd.hpp>
class instStackTriInst;
class instMan{
 public:
  instMan();
  instMan(const instMan&) = delete; // prevent copy
  instMan& operator =(const instMan&) = delete; // prevent copy
  ~instMan();
  unsigned int openHandle(bool isOverlay = false);
  void startFrame();
  void renderInst(unsigned int handle, const glm::mat4 &proj, const glm::vec3 &rgbOutline, const glm::vec3 &rgbFill);
  instStackTriInst* getIsOutline(unsigned int handle) const;
  instStackTriInst* getIsFill(unsigned int handle) const;
  void endFrame();
  void shutdown();
 protected:
  unsigned int nHandles;
  std::vector <instStackTriInst*> isOutline;
  std::vector <instStackTriInst*> isFill;
  std::vector <std::vector<glm::mat4>*> allProj;
  std::vector <std::vector<glm::vec3>*> allRgbOutline;
  std::vector <std::vector<glm::vec3>*> allRgbFill;
  std::vector <int> overlayMode;
  bool frameIsOn;
};
