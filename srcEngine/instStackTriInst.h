#pragma once
#include "myList.hpp"
#include "allMyGlIncludes.h"
#include <glm/fwd.hpp>
class instStackTriInst {
 protected:
  bool isFinalized = false;
  
  struct myGlTri {
    GLushort v1;
    GLushort v2;
    GLushort v3;
  };

  struct myGlVertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
  };

  myList<myGlTri> triList = myList<myGlTri>(10);
  myList<myGlVertex> vertexList = myList<myGlVertex>(10);
  
  // post-finalize
  GLuint idVertex;
  GLuint idTri;
  GLuint idRgb;
  GLuint idProj;
  GLuint idProgTri;
  bool isShutdown;
 public:
  instStackTriInst();
  instStackTriInst(const instStackTriInst&) = delete; // prevent copy
  instStackTriInst& operator =(const instStackTriInst&) = delete; // prevent copy
  ~instStackTriInst();
  void finalize();
  void reset();

  /** close object while openGl is still up */
  void shutdown();
  unsigned int pushVertex(glm::vec3 xyz);
  void pushTri(int v1, int v2, int v3);
  void pushTwoTri(int v1, int v2, int v3, int v4);
  void run(const glm::mat4* const proj, const glm::vec3* const rgb, int nInst) const;
  void runOverlay(const glm::mat4* const proj, const glm::vec3* const rgb, int nInst) const;
  void run1(unsigned int ixTri1, unsigned int ixTri2, const glm::mat4 &proj, const glm::vec3 &rgb) const;
  unsigned int getTriCount() const;
};
