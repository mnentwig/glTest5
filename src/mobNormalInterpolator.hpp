#pragma once
#include <glm/vec3.hpp>
#include <glm/gtx/normal.hpp> // length, cmath
#include <string>

/** purpose: interpolates the normal vector of a mob when it moves on a surface from one tri to another */
class mobNormalInterpolator {
  using std::nanf;
  using std::runtime_error;
  private:
  const std::string classname = "mobNormalInterpolator";
  public:
  /** constructor.Resulting object will interpolate between two normals with speed_dps degrees per seconds. */
  mobNormalInterpolator(float speed_dps){
    this->secondsPerDegree = 1.0f / speed_dps;
  }

  /** sets the normal to vecNormal and disables interpolation */
  void reset(glm::vec3 newNormal){
    this->state = stable;
    this->nLast = newNormal;
    this->t1 = nanf;
    this->t2 = nanf;
    this->n1 = glm::vec3 (nanf, nanf, nanf);
    this->n2 = glm::vec3 (nanf, nanf, nanf);
  }

  /** sets newNormal as new normal vector which will be reached at now_s + this.tInterp_s */
  void set(glm::vec3 newNormal, float now_s){
    if (this->state == invalid) throw new runtime_error (classname + ": not initialized");

// === calculate interpolation time from angle and speed
    float phi_rad = std::acos (glm::dot (this->n1, this->n2) / (glm::length (this->n1) * glm::length (this->n2)));
    float phi_deg = 180.0f / M_PI;
    float tInterp_s = phi_deg * this->secondsPerDegree;

    this->t1 = now_s;
    this->n1 = this->nLast;
    this->t2 = now_s + tInterp_s;
    this->n2 = newNormal;
    this->state = moving;
  }

  /** returns (possibly interpolated) normal vector at time now_s: Note: Time must be monotonic (changes state) */
  glm::vec3& getNormal(float now_s){
    switch (this->state) {
      case invalid:
        throw runtime_error (classname + ": not initialized");
      case stable:
        return this->nLast;
      case moving:
        if (now_s < this->t1) {
          this->nLast = this->n1;
          return this->nLast;
        } else if (now_s >= this->t2) {
          this->nLast = this->n2;
          this->state = stable;
          return this->nLast;
        } else {
          float f = (now_s - this->t1) / (this->t2 - this->t1);
          this->nLast = f * this->n1 + (1.0f - f) * this->n2;
          return this->nLast;
        }
      default:
        throw runtime_error (classname + ": invalid state");
    }
  }
protected:
  /** configured (constant) interpolation speed in degrees per second */
  float secondsPerDegree;
  /** last evaluated result. Used as starting point if an interpolation is interrupted */
  glm::vec3 nLast;
  /** start time in running interpolation */
  float t1 = nanf;
  /** start vector in running interpolation */
  glm::vec3 n1;
  /** end time in running interpolation */
  float t2 = nanf;
  /** end vector in running interpolation */
  glm::vec3 n2;
  enum state_e {
    invalid, moving, stable
  };
  state_e state = invalid;
};
