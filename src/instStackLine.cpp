#include <stdexcept>
#include <string.h>
#include "instStackLine.h"
#include "myGl.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
  
#define LOCATION_COORD3D 0
#define LOCATION_RGB 1
#define LOCATION_MVP_COL0 2
#define LOCATION_MVP_COL1 3
#define LOCATION_MVP_COL2 4
#define LOCATION_MVP_COL3 5
#define LOCATION_HALFVIEWPORT 6

instStackLine::instStackLine(){
  this->idProgLines = loadShaderProgFromFile("shaders/instStackLine_vs.txt", "shaders/instStackLine_fs.txt");  
  this->idLine = 0; // avoid warning
  this->idVertex = 0; // avoid warning
}
  
unsigned int instStackLine::pushVertex(glm::vec3 xyz, glm::vec3 rgb){
  assert(!this->isFinalized);
  unsigned int retVal = this->vertexList.length();
  myGlVertex v = {xyz.x, xyz.y, xyz.z, rgb.r, rgb.g, rgb.b};
  this->vertexList.push(v);
  return retVal;
}

void instStackLine::pushLine(int v1, int v2){
  assert(!this->isFinalized);
  this->lineList.push({(GLushort)v1, (GLushort)v2});
}

void instStackLine::finalize(){
  assert(!this->isFinalized);
  this->isFinalized = true;

  GL_CHK(glGenBuffers(1, &this->idVertex));
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idVertex));
  GL_CHK(glBufferData(GL_ARRAY_BUFFER, this->vertexList.getNBytes(), this->vertexList.pointer(), GL_STATIC_DRAW));
  
  GL_CHK(glGenBuffers(1, &this->idLine));
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idLine));
  GL_CHK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->lineList.getNBytes(), this->lineList.pointer(), GL_STATIC_DRAW));
    
  // === cleanup ===
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
  
void instStackLine::run(glm::mat4 proj, float viewportX, float viewportY){
  assert(this->isFinalized);
  
  GL_CHK(glLineWidth(5.0));
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
  
  // draw outlines in green
  GL_CHK(glUseProgram(this->idProgLines));
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idLine));
  GL_CHK(glDrawElements(GL_LINES, this->lineList.length()*2, GL_UNSIGNED_SHORT, 0));

  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  GL_CHK(glDisableVertexAttribArray(LOCATION_COORD3D));
  GL_CHK(glUseProgram(0));
}

void instStackLine::reset(){
  if (this->isFinalized){
    GL_CHK(glDeleteBuffers(1, &this->idLine));
    GL_CHK(glDeleteBuffers(1, &this->idVertex));
    this->vertexList.clear();
    this->lineList.clear();
    this->isFinalized = false;
  }
}

instStackLine::~instStackLine(){
  GL_CHK(glDeleteProgram(this->idProgLines));
  this->reset();
}
