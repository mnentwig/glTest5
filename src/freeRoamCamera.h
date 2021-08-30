#pragma once
#include "controllable.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "posRot.hpp"
class t1;
class freeRoamCamera: public controllable{
 public:
  freeRoamCamera(glm::vec3& pos, glm::vec3& dirFwd, glm::vec3& dirUp);
  void giveInput(fpvInput inp);// implements controllable::-
  bool getSelAttempt(glm::vec3& orig, glm::vec3& dir);// implements controllable::-
  glm::mat4 getCameraView(); // implements controllable::-
  glm::vec3 getEye() const;
  glm::vec3 getDirFwd() const;
  glm::vec3 getDirUp() const;
  void setEye(glm::vec3& eye);
  void track(controllable* mob);
  const posRot& getPosRot() const;
 protected:
  posRot core;
  float mouseSens;
  float linSpeed;
  float angSpeed;
  bool selAttempt;
};
