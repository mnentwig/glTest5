#if 0
#include <stdexcept>
#include <string.h>
#include "instStackTri.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
  
#define LOCATION_COORD3D 0
#define LOCATION_RGB 1
#define LOCATION_MVP_COL0 2
#define LOCATION_MVP_COL1 3
#define LOCATION_MVP_COL2 4
#define LOCATION_MVP_COL3 5
#define LOCATION_HALFVIEWPORT 6

instStackTri::instStackTri(){
  this->idProgTri = loadShaderProgFromFile("shaders/instStackTri_vs.txt", "shaders/instStackTri_fs.txt");
  this->isShutdown = false;
  this->idTri = 0; // avoid warning
  this->idVertex = 0; // avoid warning
}
  
unsigned int instStackTri::pushVertex(glm::vec3 xyz, glm::vec3 rgb){
  assert(!this->isFinalized);
  unsigned int retVal = this->vertexList.length();
  myGlVertex v = {xyz.x, xyz.y, xyz.z, rgb.r, rgb.g, rgb.b};
  this->vertexList.push(v);
  return retVal;
}

void instStackTri::pushTri(int v1, int v2, int v3){
  assert(!this->isFinalized);
  this->triList.push({(GLushort)v1, (GLushort)v2, (GLushort)v3});
}

void instStackTri::pushTwoTri(int v1, int v2, int v3, int v4){
  assert(!this->isFinalized);
  this->triList.push({(GLushort)v1, (GLushort)v2, (GLushort)v3});
  this->triList.push({(GLushort)v1, (GLushort)v3, (GLushort)v4});
}

void instStackTri::finalize(){
  assert(!this->isFinalized);
  this->isFinalized = true;

  GL_CHK(glGenBuffers(1, &this->idVertex));
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idVertex));
  GL_CHK(glBufferData(GL_ARRAY_BUFFER, this->vertexList.getNBytes(), this->vertexList.pointer(), GL_STATIC_DRAW));
  
  GL_CHK(glGenBuffers(1, &this->idTri));
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idTri));
  GL_CHK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->triList.getNBytes(), this->triList.pointer(), GL_STATIC_DRAW));
    
  // === cleanup ===
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
  
void instStackTri::run(glm::mat4 proj, float viewportX, float viewportY){
  assert(this->isFinalized);
  
  GL_CHK(glEnableVertexAttribArray(LOCATION_COORD3D));
  GL_CHK(glEnableVertexAttribArray(LOCATION_RGB));
  
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idVertex));
  GL_CHK(glVertexAttribPointer(LOCATION_COORD3D, 3, GL_FLOAT, GL_FALSE, sizeof(myGlVertex), 0));
  GL_CHK(glVertexAttribDivisor(LOCATION_COORD3D, 0));
  GL_CHK(glVertexAttribPointer(LOCATION_RGB, 3, GL_FLOAT, GL_FALSE, sizeof(myGlVertex), (void*)(3*sizeof(GLfloat))));
  GL_CHK(glVertexAttribDivisor(LOCATION_RGB, 0));
      
  GL_CHK(glVertexAttrib4f(LOCATION_MVP_COL0, proj[0][0], proj[0][1], proj[0][2], proj[0][3]));
  GL_CHK(glVertexAttrib4f(LOCATION_MVP_COL1, proj[1][0], proj[1][1], proj[1][2], proj[1][3]));
  GL_CHK(glVertexAttrib4f(LOCATION_MVP_COL2, proj[2][0], proj[2][1], proj[2][2], proj[2][3]));
  GL_CHK(glVertexAttrib4f(LOCATION_MVP_COL3, proj[3][0], proj[3][1], proj[3][2], proj[3][3]));
  GL_CHK(glVertexAttrib2f(LOCATION_HALFVIEWPORT, viewportX/2.0f, viewportY/2.0f));
  
  GL_CHK(glEnable(GL_POLYGON_OFFSET_FILL));
  GL_CHK(glPolygonOffset (1.0f, 1.0f));
  
  GL_CHK(glUseProgram(this->idProgTri));
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idTri));
  GL_CHK(glDrawElements(GL_TRIANGLES, this->triList.length()*3, GL_UNSIGNED_SHORT, 0));
  
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  GL_CHK(glDisableVertexAttribArray(LOCATION_COORD3D));
  GL_CHK(glDisableVertexAttribArray(LOCATION_RGB));
  GL_CHK(glUseProgram(0));
}

#if 0
// removing vertexList on finalize
bool instStackTri::checkCollision(glm::vec3 pos, glm::vec3 delta){
  float dist = glm::length(delta);
  for (unsigned int ix = 0; ix < this->triList.length(); ++ix){
    myGlTri t = this->triList[ix];
    myGlVertex v1 = this->vertexList[t.v1];
    myGlVertex v2 = this->vertexList[t.v2];
    myGlVertex v3 = this->vertexList[t.v3];
    
    glm::vec3 p1 = glm::vec3(v1.x, v1.y, v1.z);
    glm::vec3 p2 = glm::vec3(v2.x, v2.y, v2.z);
    glm::vec3 p3 = glm::vec3(v3.x, v3.y, v3.z);
    glm::vec2 isBary;
    GLfloat d;
    if (glm::intersectRayTriangle(pos, delta, p1, p2, p3, isBary, d)){
      if ((d > 0) && (d < dist))
	return true;
    }
  }
  return false;
}
#endif

void instStackTri::reset(){
  if (this->isFinalized){
    GL_CHK(glDeleteBuffers(1, &this->idTri));
    GL_CHK(glDeleteBuffers(1, &this->idVertex));
    this->vertexList.clear();
    this->triList.clear();
    this->isFinalized = false;
  }
}

void instStackTri::shutdown(){
  assert(!this->isShutdown);
  this->reset();
  GL_CHK(glDeleteProgram(this->idProgTri));
  this->isShutdown = true;
}

instStackTri::~instStackTri(){
  assert(this->isShutdown);
}
#endif
