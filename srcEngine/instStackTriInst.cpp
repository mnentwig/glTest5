#include "myGl.h"
#include <stdexcept>
#include <string.h>
#include "instStackTriInst.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
  
#define LOCATION_COORD3D 0
#define LOCATION_RGB 1
#define LOCATION_MVP_COL0 2
#define LOCATION_MVP_COL1 3
#define LOCATION_MVP_COL2 4
#define LOCATION_MVP_COL3 5

instStackTriInst::instStackTriInst(){
  this->idProgTri = loadShaderProgFromFile("shaders/instStackTri_vs.txt", "shaders/instStackTri_fs.txt");
  this->isShutdown = false;
  this->idProj = 0; // avoid warning
  this->idVertex = 0; // avoid warning
  this->idTri = 0; // avoid warning
  this->idRgb = 0; // avoid warning
}
  
unsigned int instStackTriInst::pushVertex(glm::vec3 xyz){
  assert(!this->isFinalized);
  unsigned int retVal = this->vertexList.length();
  myGlVertex v = {xyz.x, xyz.y, xyz.z};
  this->vertexList.push(v);
  return retVal;
}

void instStackTriInst::pushTri(int v1, int v2, int v3){
  assert(!this->isFinalized);
  this->triList.push({(GLushort)v1, (GLushort)v2, (GLushort)v3});
}

unsigned int instStackTriInst::getTriCount() const{
  assert(!this->isFinalized);
  return this->triList.length();
}

void instStackTriInst::pushTwoTri(int v1, int v2, int v3, int v4){
  assert(!this->isFinalized);
  this->triList.push({(GLushort)v1, (GLushort)v2, (GLushort)v3});
  this->triList.push({(GLushort)v1, (GLushort)v3, (GLushort)v4});
}

void instStackTriInst::finalize(){
  assert(!this->isFinalized);
  this->isFinalized = true;

  GL_CHK(glGenBuffers(1, &this->idRgb));
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idRgb));

  GL_CHK(glGenBuffers(1, &this->idProj));
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idProj));

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
  
void instStackTriInst::runOverlay(const glm::mat4* const proj, const glm::vec3* const rgb, int nInst) const{
  GL_CHK(glDisable (GL_DEPTH_TEST));
  this->run(proj, rgb, nInst);
  GL_CHK(glEnable (GL_DEPTH_TEST));
}

void instStackTriInst::run(const glm::mat4* const proj, const glm::vec3* const rgb, int nInst) const{
  assert(this->isFinalized);
  if (this->triList.length() < 1) return;
  if (nInst < 1) return;

  // === COORD3D: per triangle ===
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idVertex));
  GL_CHK(glEnableVertexAttribArray(LOCATION_COORD3D));
  GL_CHK(glVertexAttribPointer(LOCATION_COORD3D, 3, GL_FLOAT, GL_FALSE, sizeof(myGlVertex), 0));
  GL_CHK(glVertexAttribDivisor(LOCATION_COORD3D, 0));

  // === RGB: per instance ===
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idRgb));
  GL_CHK(glBufferData(GL_ARRAY_BUFFER, nInst * sizeof(glm::vec3), (void*)rgb, GL_STATIC_DRAW));
  GL_CHK(glEnableVertexAttribArray(LOCATION_RGB));
  GL_CHK(glVertexAttribPointer(LOCATION_RGB, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0));
  GL_CHK(glVertexAttribDivisor(LOCATION_RGB, 1));

  // === proj: per instance (4 rows) ===
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idProj));
  GL_CHK(glBufferData(GL_ARRAY_BUFFER, nInst * sizeof(glm::mat4), (void*)proj, GL_STATIC_DRAW));
   
  GL_CHK(glEnableVertexAttribArray(LOCATION_MVP_COL0));
  GL_CHK(glVertexAttribPointer(LOCATION_MVP_COL0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0*sizeof(glm::vec4))));
  GL_CHK(glVertexAttribDivisor(LOCATION_MVP_COL0, 1));

  GL_CHK(glEnableVertexAttribArray(LOCATION_MVP_COL1));
  GL_CHK(glVertexAttribPointer(LOCATION_MVP_COL1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1*sizeof(glm::vec4))));
  GL_CHK(glVertexAttribDivisor(LOCATION_MVP_COL1, 1));

  GL_CHK(glEnableVertexAttribArray(LOCATION_MVP_COL2));
  GL_CHK(glVertexAttribPointer(LOCATION_MVP_COL2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2*sizeof(glm::vec4))));
  GL_CHK(glVertexAttribDivisor(LOCATION_MVP_COL2, 1));

  GL_CHK(glEnableVertexAttribArray(LOCATION_MVP_COL3));
  GL_CHK(glVertexAttribPointer(LOCATION_MVP_COL3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3*sizeof(glm::vec4))));
  GL_CHK(glVertexAttribDivisor(LOCATION_MVP_COL3, 1));
  
  // === program ===
  GL_CHK(glUseProgram(this->idProgTri));

  // === vertex indices ===
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idTri));

  // === draw ===
  GL_CHK(glDrawElementsInstanced(GL_TRIANGLES, this->triList.length()*3, GL_UNSIGNED_SHORT, 0, nInst));
  
  // === cleanup ===
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  GL_CHK(glDisableVertexAttribArray(LOCATION_COORD3D));
  GL_CHK(glDisableVertexAttribArray(LOCATION_RGB));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL0));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL1));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL2));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL3));
  GL_CHK(glUseProgram(0));
}

void instStackTriInst::run1(unsigned int ixTri1, unsigned int ixTri2, const glm::mat4 &proj, const glm::vec3 &rgb) const{
  assert(this->isFinalized);

  // === COORD3D: per triangle ===
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idVertex));
  GL_CHK(glEnableVertexAttribArray(LOCATION_COORD3D));
  GL_CHK(glVertexAttribPointer(LOCATION_COORD3D, 3, GL_FLOAT, GL_FALSE, sizeof(myGlVertex), 0));
  GL_CHK(glVertexAttribDivisor(LOCATION_COORD3D, 0));

  // === RGB ===
  GL_CHK(glDisableVertexAttribArray(LOCATION_RGB));
  GL_CHK(glVertexAttrib3fv(LOCATION_RGB, (float*)&rgb));

  // === proj ===   
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL0));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL1));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL2));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL3));
  GL_CHK(glVertexAttrib4fv(LOCATION_MVP_COL0, &proj[0][0]));
  GL_CHK(glVertexAttrib4fv(LOCATION_MVP_COL1, &proj[1][0]));
  GL_CHK(glVertexAttrib4fv(LOCATION_MVP_COL2, &proj[2][0]));
  GL_CHK(glVertexAttrib4fv(LOCATION_MVP_COL3, &proj[3][0]));
  
  // === program ===
  GL_CHK(glUseProgram(this->idProgTri));

  // === vertex indices ===
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idTri));

  // === draw ===
  // note: https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glDrawElements.xhtml "indices specifies a byte offset..."
  unsigned long long byteOffset = ixTri1*3*sizeof(GLshort);
  GL_CHK(glDrawElements(GL_TRIANGLES, (ixTri2-ixTri1)*3, GL_UNSIGNED_SHORT, (void*)byteOffset));
  
  // === cleanup ===
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  GL_CHK(glDisableVertexAttribArray(LOCATION_COORD3D));
  GL_CHK(glUseProgram(0));
}

void instStackTriInst::reset(){
  if (this->isFinalized){
    GL_CHK(glDeleteBuffers(1, &this->idTri));
    GL_CHK(glDeleteBuffers(1, &this->idVertex));
    this->vertexList.clear();
    this->triList.clear();
    this->isFinalized = false;
  }
}

void instStackTriInst::shutdown(){
  assert(!this->isShutdown);
  this->isShutdown = true;
  this->reset();
  GL_CHK(glDeleteProgram(this->idProgTri));
}

instStackTriInst::~instStackTriInst(){
  assert(this->isShutdown);
}
