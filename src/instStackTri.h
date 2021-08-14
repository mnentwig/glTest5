#pragma once
#if 0
#include "myGl.h"
#include "myList.hpp"
#include <glm/glm.hpp>
class instStackTri {
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
    GLfloat r;
    GLfloat g;
    GLfloat b;
  };
  
  myList<myGlTri> triList = myList<myGlTri>(10);
  myList<myGlVertex> vertexList = myList<myGlVertex>(10);
  
  // post-finalize
  GLuint idVertex;
  GLuint idTri;
  GLuint idProgTri;
  bool isShutdown;
 public:
  instStackTri();
  ~instStackTri();
  unsigned int pushVertex(glm::vec3 xyz, glm::vec3 rgb);
  void pushTri(int v1, int v2, int v3);
  void pushTwoTri(int v1, int v2, int v3, int v4);
  void finalize();  
  void run(glm::mat4 proj, float viewportX, float viewportY);
  void reset();
  void shutdown();
  //bool checkCollision(glm::vec3 p1, glm::vec3 p2);
};
#endif
