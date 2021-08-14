#include "crosshairs.h"
#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "instMan.h"
#include "outliner.hpp"
#include "utils.h"
unsigned int crosshairs::imHandle;
instMan *crosshairs::im;
void crosshairs::startup(instMan* im){
  crosshairs::im = im;
  imHandle = im->openHandle (/*isOverlay*/true);

  std::vector<glm::vec3> vvNE;
#if 0
  vv.push_back (glm::vec3 (-15, -15, 0));
  vv.push_back (glm::vec3 (-15, 15, 0));
  vv.push_back (glm::vec3 (15, 15, 0));
  vv.push_back (glm::vec3 (15, -15, 0));
  outliner::generateOutlinedShape (vv.data (), vv.size(), 0.03f, im->getIsOutline (imHandle), im->getIsFill (imHandle));
#endif
  float a = 0.2f;
  float b = 5;
  vvNE.push_back (glm::vec3 ( a, a, 0));
  vvNE.push_back (glm::vec3 ( a, b, 0));
  vvNE.push_back (glm::vec3 ( b, a, 0));

  float width = 0.05f;
  std::vector<glm::vec3> vv;
  vv = scaleXYZ(vvNE, 1,  1, 1);
  outliner::generateOutlinedShape (vv.data (), vv.size(), width, im->getIsOutline (imHandle), NULL);
#if 1
  vv = scaleXYZ(vvNE, 1, -1, 1);
  outliner::generateOutlinedShape (vv.data (), vv.size(), width, im->getIsOutline (imHandle), NULL);
  vv = scaleXYZ(vvNE, -1, 1, 1);
  outliner::generateOutlinedShape (vv.data (), vv.size(), width, im->getIsOutline (imHandle), NULL);
  vv = scaleXYZ(vvNE, -1, -1, 1);
  outliner::generateOutlinedShape (vv.data (), vv.size(), width, im->getIsOutline (imHandle), NULL);
#endif
  im->getIsOutline (imHandle)->finalize ();
  im->getIsFill (imHandle)->finalize ();
}

crosshairs::crosshairs(){
}
void crosshairs::render(glm::mat4& proj){
  im->renderInst (imHandle, proj, glm::vec3 (1, 1, 1), glm::vec3 (0.5, 0.5, 0.5));
}
