#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include "../srcEngine/engine.h"
#include "../srcEngine/instMan.h"
#include "../srcEngine/explosible.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include "../srcEngine/glTypes.h"

explosible* generateTestcube(float edgeLen, float width, instMan* im){
  explosible *ex = new explosible (im);
  float e = 0.5*edgeLen;
  std::vector<glm::vec3> v1;
  v1.push_back (glm::vec3 (-e, -e, -e));
  v1.push_back (glm::vec3 (-e,  e, -e));
  v1.push_back (glm::vec3 ( e,  e, -e));
  v1.push_back (glm::vec3 ( e, -e, -e));
  std::vector<glm::vec3> v2;
  v2.push_back (glm::vec3 (-e, -e,  e));
  v2.push_back (glm::vec3 (-e,  e,  e));
  v2.push_back (glm::vec3 ( e,  e,  e));
  v2.push_back (glm::vec3 ( e, -e,  e));

  unsigned int nV = v1.size ();
  assert(v2.size () == nV);

  ex->generateOutlinedShape (v1.data (), nV, width);
  ex->generateOutlinedShape (v2.data (), nV, width);
  ex->generateOutlinedBody (v1.data (), v2.data (), nV, width);
  ex->finalize ();
  return ex;
}

int main(void){
  instMan im;
  srand (0);
#ifdef NDEBUG
  std::cout << "NDEBUG set - assert disabled" << std::endl;
  assert(0);
#endif

  engine::engine e;
  e.startup();

  explosible* testcube = generateTestcube(0.8f, 0.05f, &im);

  while (true) {
    const engine::preDrawState* pds = e.preDraw();
    if (pds->windowClose)break;
    e.beginDraw();

    glm::vec3 eye(0, 5, 0);
    glm::vec3 far(0, 5, -1);
    glm::vec3 up(0, 1, 0);
    glm::mat4 view = glm::lookAt (eye, far, up);

    glm::mat4 proj = glm::perspective (45.0f, 1.0f * e.screenWidth / e.screenHeight, 0.01f,
                                       1000.0f) * view;

    im.startFrame ();
    glm::vec3 rgbOuter(0, 1, 0);
    glm::vec3 rgbInner(0.1, 0.1, 0.1);
    float phi = pds->time_s * 2.0*M_PI/4;

    glm::mat4 rot = glm::rotate (glm::mat4 (1.0f), phi, glm::vec3(0, 1, 0));


    for (float x = -10; x <= 10; ++x){
      for (float z = -20; z <= 10; ++z){
        //glm::mat4 v = glm::inverse (glm::lookAt (glm::vec3 (0, 0, 0), this->dirFwd, dirUp));
        glm::mat4 v = glm::translate (glm::mat4 (1.0f), glm::vec3(x, 0, z)) * rot;
        glm::mat4 projT = proj * v;
        testcube->render(projT, rgbOuter, rgbInner);
      }
    }

    im.endFrame ();
    e.endDraw();
  }
  im.shutdown();
  e.shutdown();
  std::cout << "main Done" << std::endl;
  return EXIT_SUCCESS;
}
