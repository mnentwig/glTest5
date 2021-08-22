#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include <cmath>

//** rotation and orientation
class posRot {
public:
  posRot(const glm::vec3& pos, const glm::vec3& dirFwd, const glm::vec3& dirUp){
    this->pos = pos;
    this->dirFwd = dirFwd;
    this->dirUp = dirUp;
    this->updateDirLat ();
    this->assertSanity ();
  }

  const glm::vec3& getDirFwd() const{
    return this->dirFwd;
  }

  const glm::vec3& getDirLat() const{
    return this->dirLat;
  }

  const glm::vec3& getDirUp() const{
    return this->dirUp;
  }

  const glm::vec3& getPos() const{
    return this->pos;
  }

  void setPos(const glm::vec3& pos){
    this->pos = pos;
  }

  void setPosFwdUp(const glm::vec3& pos, const glm::vec3& dirFwd, const glm::vec3& dirUp){
    this->pos = pos;
    this->dirFwd = dirFwd;
    this->dirUp = dirUp;
    this->updateDirLat ();
    this->assertSanity ();
  }

//** applies rotation (intended for camera control by user input, not mathematical exactness) //
  void rotate(float dYaw, float dPitch, float dRoll){
    this->assertSanity ();

// determine axis of combined rotation
    glm::vec3 axis = dYaw * this->dirUp + dPitch * this->dirLat + dRoll * this->dirFwd;
    float f = glm::length (axis);// happens to be the combined rotation (axes are orthogonal => sqrt(dYaw^2+dPitch^2+dRoll^2))
    if (fabs (f) < 1e-6)
    return;
    axis /= f;// normalize

// rotate
    glm::mat3 rot = glm::rotate (glm::mat4 (1.0f), f, axis);
    this->dirFwd = this->dirFwd * rot;
    this->dirUp = this->dirUp * rot;
    this->dirLat = this->dirLat * rot;
  }

  glm::mat4 model2world() const{
    glm::mat4 v = glm::inverse (glm::lookAt (glm::vec3 (0, 0, 0), this->dirFwd, this->dirUp));
    v = glm::translate (glm::mat4 (1.0f), this->pos) * v;
//    return glm::translate (glm::mat4 (1.0f), this->pos);
    return v;
  }

  void move(float forw, float up, float lat){
    this->pos += this->dirFwd * forw + this->dirUp * up + this->dirLat * lat;
  }

  void move(const glm::vec3& vec){
    this->pos += vec;
  }

  glm::mat4 getView() const{
    return glm::lookAt (this->pos, this->pos + this->dirFwd, this->dirUp);
  }
protected:
  void updateDirLat(){
    this->dirLat = glm::normalize (glm::cross (this->dirFwd, this->dirUp));
  }
  void assertSanity() const{
    const float eps = 1e-3f;
// === check unity direction vectors ===
    assert(std::abs (1.0f - glm::length2 (this->dirFwd)) < eps);
    assert(std::abs (1.0f - glm::length2 (this->dirUp)) < eps);
    assert(std::abs (1.0f - glm::length2 (this->dirLat)) < eps);

// === check orthogonality  ===
    assert(std::abs (1.0f - glm::length2 (glm::cross (this->dirLat, this->dirUp))) < eps);
    assert(std::abs (1.0f - glm::length2 (glm::cross (this->dirUp, this->dirFwd))) < eps);
    assert(std::abs (1.0f - glm::length2 (glm::cross (this->dirFwd, this->dirUp))) < eps);
  }

//** position (reference point) */
  glm::vec3 pos;

//** orientation: forwards direction */
  glm::vec3 dirFwd;

//** orientation: up direction */
  glm::vec3 dirUp;

//** orientation: left/right direction */
  glm::vec3 dirLat;
};
