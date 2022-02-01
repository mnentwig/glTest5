#pragma once
#include <string>
#include <vector>
#include <glm/fwd.hpp>
class instStackTriInst;

class instTemplate;
class instMan{
 public:
  instMan();
  instMan(const instMan&) = delete; // prevent copy
  instMan& operator =(const instMan&) = delete; // prevent copy
  ~instMan();
  unsigned int openHandle(unsigned int nCol, bool isOverlay);
  void startFrame();
  void renderInst(unsigned int handle, const glm::mat4 &proj, const std::vector<glm::vec3> rgb);
  instStackTriInst* getIsti(unsigned int handle, int ixCol) const;
  unsigned int getNCol(unsigned int handle) const;
  void endFrame();
  void shutdown();
 protected:
  unsigned int nHandles;
  std::vector <instTemplate*> templates;

  bool frameIsOn;
};
