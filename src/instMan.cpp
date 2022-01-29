#define GLM_FORCE_RADIANS
#include "instMan.h"
#include <glm/glm.hpp>
#include "instStackTriInst.h"
#include <iostream>

// TODO: Make number of colors variable instead of outline / fill
instMan::instMan(){
  this->nHandles = 0;
  this->frameIsOn = false;
}
unsigned int instMan::openHandle(bool isOverlay){
  this->allProj.push_back(new std::vector<glm::mat4>);
  this->allRgbOutline.push_back(new std::vector<glm::vec3>);
  this->allRgbFill.push_back(new std::vector<glm::vec3>);
  this->isOutline.push_back(new instStackTriInst());
  this->isFill.push_back(new instStackTriInst());
  this->overlayMode.push_back(isOverlay ? 1 : 0);

  return this->nHandles++;
}

instStackTriInst* instMan::getIsOutline(unsigned int handle) const{
  return this->isOutline[handle];
}

instStackTriInst* instMan::getIsFill(unsigned int handle) const{
  return this->isFill[handle];
}

void instMan::startFrame(){
  assert(!this->frameIsOn);
  this->frameIsOn = true;

  for (unsigned int ix = 0; ix < this->nHandles; ++ix){
    this->allProj[ix]->clear();
    this->allRgbOutline[ix]->clear();
    this->allRgbFill[ix]->clear();
  }
}

void instMan::renderInst(unsigned int handle, const glm::mat4 &proj, const glm::vec3 &rgbOutline, const glm::vec3 &rgbFill){
  assert(this->frameIsOn);

  this->allProj[handle]->push_back(proj);
  this->allRgbOutline[handle]->push_back(rgbOutline);
  this->allRgbFill[handle]->push_back(rgbFill);  
}

void instMan::endFrame(){
  assert(this->frameIsOn);
  this->frameIsOn = false;

  for (unsigned int ix = 0; ix < this->nHandles; ++ix){
    glm::mat4* proj = this->allProj[ix]->data();
    int nInst = this->allProj[ix]->size();
    if (this->overlayMode[ix]){
      this->isOutline[ix]->runOverlay(proj, this->allRgbOutline[ix]->data(), nInst);
      this->isFill[ix]->runOverlay(proj, this->allRgbFill[ix]->data(), nInst);
    } else {
      this->isOutline[ix]->run(proj, this->allRgbOutline[ix]->data(), nInst);
      this->isFill[ix]->run(proj, this->allRgbFill[ix]->data(), nInst);
    }
  }
}

void instMan::shutdown(){
  for (unsigned int ix = 0; ix < this->nHandles; ++ix){
    this->isOutline[ix]->shutdown();
    this->isFill[ix]->shutdown();
  }
}

instMan::~instMan(){
  for (unsigned int ix = 0; ix < this->nHandles; ++ix){
    delete this->allProj[ix];
    delete this->allRgbOutline[ix];
    delete this->allRgbFill[ix];
    delete this->isOutline[ix];
    delete this->isFill[ix];
  }
}
