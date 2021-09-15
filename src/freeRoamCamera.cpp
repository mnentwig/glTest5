#include "freeRoamCamera.h"
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
freeRoamCamera::freeRoamCamera(glm::vec3& pos, glm::vec3& dirFwd, glm::vec3& dirUp) : core (pos, dirFwd, dirUp){
  this->mouseSens = 0.06f * M_PI / 180.0f;
  this->linSpeed = 50.0f;
  this->angSpeed = 120.0f * M_PI / 180.0f;
  this->selAttempt = false;
}

void freeRoamCamera::giveInput(fpvInput inp, controllable* selectedMob){
  float dt = inp.deltaTime_s;
  this->selAttempt |= inp.rightButtonChangeDown;
  float dxdt = 0;
  float dydt = 0;
  float dzdt = 0;

  if (selectedMob == NULL) {
// move camera
    if (inp.key_strafeLeft) dxdt -= linSpeed;
    if (inp.key_strafeRight) dxdt += linSpeed;
    if (inp.key_forw) dydt += linSpeed;
    if (inp.key_backw) dydt -= linSpeed;
    if (inp.key_up) dzdt -= linSpeed;
    if (inp.key_down) dzdt += linSpeed;

    float forw = 0;
    float up = 0;
    float lat = 0;
// move camera
    forw = dt * dydt / 2.0f;
    up = dt * dzdt / 2.0f;
    lat = dt * dxdt / 2.0f;

// === half movement pre-rotation ===
    this->core.move (forw, up, lat);

// === rotation ===
    this->giveInput_rotate (inp);

// === half movement post-rotation ===
    this->core.move (forw, up, lat);
  } else {
    selectedMob->giveInput(inp);
    this->giveInput_rotate (inp);

    const posRot &mobCore = selectedMob->getPosRot ();

    // depends on normal - glitches
    //    this->core.setPos (mobCore.getPos () + 6.0f * mobCore.getDirUp () - 0.0f * this->core.getDirFwd ());

    // uses principal axis - doesn't glitch
    this->core.setPos (mobCore.getPos () + 6.0f * glm::vec3(0.0f, 1.0f, 0.0f));
  }
}

void freeRoamCamera::giveInput_rotate(fpvInput inp){
  float dt = inp.deltaTime_s;
  float dyawdt = 0;
  float dpitchdt = 0;
  float drolldt = 0;
  if (inp.key_cameraYawLeft) dyawdt += angSpeed;
  if (inp.key_cameraYawRight) dyawdt -= angSpeed;
  if (inp.key_cameraPitchUp) dpitchdt += angSpeed;
  if (inp.key_cameraPitchDown) dpitchdt -= angSpeed;
  if (inp.key_cameraRollLeft) drolldt -= angSpeed;
  if (inp.key_cameraRollRight) drolldt += angSpeed;

  float dyaw = mouseSens * (inp.mouseDeltaX);
  float dpitch = mouseSens * (inp.mouseDeltaY);
  float droll = 0;

  this->core.rotate (dyawdt * dt + dyaw, dpitchdt * dt + dpitch, drolldt * dt + droll);
}

glm::vec3 freeRoamCamera::getEye() const{
  return this->core.getPos ();
}

glm::vec3 freeRoamCamera::getDirFwd() const{
  return this->core.getDirFwd ();
}

glm::vec3 freeRoamCamera::getDirUp() const{
  return this->core.getDirUp ();
}

void freeRoamCamera::setEye(glm::vec3& eye){
  this->core.setPos (eye);
//  this->eye = eye;
}

bool freeRoamCamera::getSelAttempt(glm::vec3& orig, glm::vec3& dir){
  if (this->selAttempt) {
    this->selAttempt = false;
    orig = this->core.getPos ();
    dir = 1000.0f * this->core.getDirFwd ();// FIXME hardcoded constant
    return true;
  } else {
    return false;
  }
}

glm::mat4 freeRoamCamera::getCameraView(){
  return this->core.getView ();
}

const posRot& freeRoamCamera::getPosRot() const{
  return this->core;
}

