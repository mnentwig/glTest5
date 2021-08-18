#pragma once
#include "glewTypes.h"
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include "terrTriDomain.h"
class terrTri;

//** manages openGl rendering of map surface */
class terrain {
public:
  terrain(const char* filename, terrTriDomain& d);
  void render(glm::mat4& proj, glm::vec3 camera, float maxRenderDistance, glm::vec3& colOdd, glm::vec3& colEven);

protected:
  unsigned int nY;
  unsigned int nX;
  unsigned int nTriOdd;
  unsigned int nTriEven;
  GLuint idTriOdd;
  GLuint idTriEven;
  GLuint idProg;
  glm::vec3 getVertex(unsigned short* data, unsigned int ixX, unsigned int ixY);
  terrTriVertIx getVertexIx(unsigned int ixX, unsigned int ixY);
};
