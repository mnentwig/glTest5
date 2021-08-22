#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/projection.hpp>

#include "terrTri.h"
#include "terrTriDomain.h"
/** manages movement along the surface of a terrTriDomain */
class terrTriTracker {
public:
  terrTriTracker(terrTriDomain* ttd){
    this->ttd = ttd;
    this->trackedTri = NULL;
  }

//** initializes terrTriTracker with point p that gets projected on surface.
// returns true if successful (p becomes projected point) or false otherwise (p remains unchanged)
  bool initialize(posRot& core){
// === fast intersection in 2D ===
    terrTri *t = this->ttd->locateTriByVerticalProjection (core.getPos ());
    if (t) {
// === setup triangle pts ===
      glm::vec3 v0;
      glm::vec3 v1;
      glm::vec3 v2;
      t->getV012 (ttd, v0, v1, v2);
      const glm::vec3 dirUpDown = glm::vec3 (0.0f, 1.0f, 0.0f);

// === intersect in 3D ===
      glm::vec2 isBary;
      float d;
      bool intersectSuccess = glm::intersectRayTriangle (core.getPos (), dirUpDown, v0, v1, v2, isBary, d);
      assert(intersectSuccess);
      glm::vec3 newPos = v0 + isBary[0] * (v1 - v0) + isBary[1] * (v2 - v0);

      this->placeOnTri (t, newPos, core);
      return true;
    }
    return false;
  }

  // TODO split xy functions into geomUtils2d.hpp
  // TODO keep copy of core instead of param
  // TODO create projectMobVector() in terrTri, move out here

  /** moves a distance dist (can be negative) in the direction of dirFwd (projected onto map).
   * returns new dirFwd and dirUp */
  void track(posRot& core, float dist){
    glm::vec3 v0_3d;
    glm::vec3 v1_3d;
    glm::vec3 v2_3d;

// === project triangle on constant-z plane ===
    this->trackedTri->getV012 (this->ttd, v0_3d, v1_3d, v2_3d);
    glm::vec3 v0_constZ = this->threeDToConstZ * v0_3d;
    glm::vec3 v1_constZ = this->threeDToConstZ * v1_3d;
    glm::vec3 v2_constZ = this->threeDToConstZ * v2_3d;

    glm::vec3 posStart_constZ = this->threeDToConstZ * core.getPos ();
    glm::vec3 dirFwd_constZ = this->threeDToConstZ * core.getDirFwd ();
    glm::vec3 posEnd_constZ = posStart_constZ + dirFwd_constZ * dist;
    if (pointInTriangleXY (posEnd_constZ, v0_constZ, v1_constZ, v2_constZ)) {
      core.move (dist * core.getDirFwd ());
    } else {
      glm::vec3 v0_constZ = this->threeDToConstZ * v0_3d;
      glm::vec3 v1_constZ = this->threeDToConstZ * v1_3d;
      glm::vec3 v2_constZ = this->threeDToConstZ * v2_3d;

// TODO three checks have many common subexpressions
      glm::vec3 intersection_constZ;
      terrTri *neighbor = NULL;
      if (calcLineLineIntersectionXY (v0_constZ, v1_constZ, posStart_constZ, posEnd_constZ, intersection_constZ)) {
        neighbor = this->trackedTri->getNeighbor01 ();
      } else if (calcLineLineIntersectionXY (v1_constZ, v2_constZ, posStart_constZ, posEnd_constZ, intersection_constZ)) {
        neighbor = this->trackedTri->getNeighbor12 ();
      } else if (calcLineLineIntersectionXY (v2_constZ, v0_constZ, posStart_constZ, posEnd_constZ, intersection_constZ)) {
        neighbor = this->trackedTri->getNeighbor20 ();
      }
      assert(neighbor);
      glm::vec3 intersection_3d = intersection_constZ * this->constZToThreeD;
      this->placeOnTri (neighbor, intersection_3d, core);
      dist -= glm::distance (posStart_constZ, intersection_constZ);
      if (dist > 1e-3) {
        this->track (core, dist);// note, loop via recursion
      }
    }
  }
protected:
  terrTriDomain *ttd;
  terrTri *trackedTri;
  //*** rotation matrix for tri plane: 2d xy back to 3d */
  glm::mat3 constZToThreeD;
  //*** rotation matrix for tri plane: 3d to 2d xyz */
  glm::mat3 threeDToConstZ;

  static float triCheckSign(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3){
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
  }

//** checks whether pt lies in triangle v1, v2, v3 in 2D (only considering dimensions dim1, dim2 with 012=xyz) /*
  static bool pointInTriangleXY(const glm::vec3& pt, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3){
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = triCheckSign (pt, v1, v2);
    d2 = triCheckSign (pt, v2, v3);
    d3 = triCheckSign (pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
  }

  static bool calcLineLineIntersectionXY(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3& outIntersection){
// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection

// === check coplanarity ===
    const float eps = 1e-6;
    assert(std::fabs (v1.z - v0.z) < eps);
    assert(std::fabs (v2.z - v0.z) < eps);
    assert(std::fabs (v3.z - v0.z) < eps);

    float x1 = v0.x;
    float y1 = v0.y;
    float x2 = v1.x;
    float y2 = v1.y;
    float x3 = v2.x;
    float y3 = v2.y;
    float x4 = v3.x;
    float y4 = v3.y;

    float numX = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
    float numY = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);
    float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    outIntersection.x = numX / denom;
    outIntersection.y = numY / denom;
    assert(std::fabs (denom) > eps);
    outIntersection.z = v0.z;// any v because coplanar

// === check range ===
    if (((outIntersection.x < v0.x) && (outIntersection.x < v1.x))
        || ((outIntersection.x < v2.x) && (outIntersection.x < v3.x))

        || ((outIntersection.y < v0.y) && (outIntersection.y < v1.y))
        || ((outIntersection.y < v2.y) && (outIntersection.y < v3.y))

        || ((outIntersection.x > v0.x) && (outIntersection.x > v1.x))
        || ((outIntersection.x > v2.x) && (outIntersection.x > v3.x))

        || ((outIntersection.y > v0.y) && (outIntersection.y > v1.y))
        || ((outIntersection.y > v2.y) && (outIntersection.y > v3.y))) {
      return false;
    }
    return true;
  }

  void placeOnTri(terrTri* t, glm::vec3& newPos, posRot& core){

    glm::vec3 prevDirFwd = core.getDirFwd ();
    glm::vec3 prevDirUp = core.getDirUp ();

// === get plane and normal of intersected tri ===
    glm::vec3 triUnityDirPlane1;
    glm::vec3 triUnityDirPlane2;
    glm::vec3 triUnityDirNorm;
    t->getAxes (this->ttd, triUnityDirPlane1, triUnityDirPlane2, triUnityDirNorm);
// TODO: use orig minus projection on normal
    glm::vec3 newDirFwd =
        glm::proj (prevDirFwd, triUnityDirPlane1) +
            glm::proj (prevDirFwd, triUnityDirPlane2);
    newDirFwd /= glm::length (newDirFwd);

// === hemisphere of up direction remains unchanged ===
    glm::vec3 newDirUp = triUnityDirNorm;
    if (glm::dot (triUnityDirNorm, prevDirUp) < 0.0f) {
      newDirFwd *= -1.0f;
      newDirUp *= -1.0f;
    }

// === assign new position and inclination ===
    core.setPosFwdUp (newPos, newDirFwd, newDirUp);
    this->trackedTri = t;

//** from xy plane with constant z to original 3D space */
    this->constZToThreeD[0] = glm::normalize (glm::cross (newDirFwd, newDirUp));// x maps to lateral axis
    this->constZToThreeD[1] = newDirFwd;// y maps to fwd direction
    this->constZToThreeD[2] = newDirUp;// z maps to y direction

// ** from 3D to xy with constant z => inverse of above */
    this->threeDToConstZ = glm::inverse (this->constZToThreeD);
  }
};
