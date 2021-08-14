#pragma once
#include "myList.hpp"
#include "glewTypes.h"
#include <glm/fwd.hpp>

class instStackLine {
 protected:
  bool isFinalized = false;
  
  struct myGlLine {
    GLushort v1;
    GLushort v2;
  };

  struct myGlVertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
  };
 
  myList<myGlLine> lineList = myList<myGlLine>(10);
  myList<myGlVertex> vertexList = myList<myGlVertex>(10);
  
  // post-finalize
  GLuint idVertex;
  GLuint idLine;
  GLuint idProgLines;
 public:
  instStackLine();
  ~instStackLine();
  unsigned int pushVertex(glm::vec3 xyz, glm::vec3 rgb);
  void pushLine(int v1, int v2);
  void finalize();  
  void run(glm::mat4 proj, float viewportX, float viewportY);
  void reset();
};
