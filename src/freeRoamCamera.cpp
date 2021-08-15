#include "freeRoamCamera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
freeRoamCamera::freeRoamCamera(glm::vec3& pos, glm::vec3& dirFwd, glm::vec3& dirUp) : core (pos, dirFwd, dirUp){
  this->mouseSens = 0.06f * M_PI / 180.0f;
  this->linSpeed = 50.0f;
  this->angSpeed = 120.0f * M_PI / 180.0f;
  this->selAttempt = false;
}

void freeRoamCamera::giveInput(fpvInput inp){
  float dt = inp.deltaTime_s;
  float dxdt = 0;
  float dydt = 0;
  float dzdt = 0;
  if (inp.key_strafeLeft) dxdt -= linSpeed;
  if (inp.key_strafeRight) dxdt += linSpeed;
  if (inp.key_forw) dydt += linSpeed;
  if (inp.key_backw) dydt -= linSpeed;
  if (inp.key_up) dzdt -= linSpeed;
  if (inp.key_down) dzdt += linSpeed;

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
  this->selAttempt |= inp.rightButtonChangeDown;

  float forw = dt * dydt / 2.0f;
  float up = dt * dzdt / 2.0f;
  float lat = dt * dxdt / 2.0f;

// === half movement pre-rotation ===
  this->core.move (forw, up, lat);

// === rotation ===
  this->core.rotate (dyawdt * dt + dyaw, dpitchdt * dt + dpitch, drolldt * dt + droll);

// === half movement post-rotation ===
  this->core.move (forw, up, lat);
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
    dir = 1000.0f * this->core.getDirFwd (); // FIXME hardcoded constant
    return true;
  } else {
    return false;
  }
}

glm::mat4 freeRoamCamera::getCameraView(){
  return this->core.getView ();
}
