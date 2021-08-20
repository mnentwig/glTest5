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

// === get plane and normal of intersected tri ===
      glm::vec3 triUnityDirPlane1;
      glm::vec3 triUnityDirPlane2;
      glm::vec3 triUnityDirNorm;
      t->getAxes (this->ttd, triUnityDirPlane1, triUnityDirPlane2, triUnityDirNorm);
      // TODO: use orig minus projection on normal
      glm::vec3 newDirFwd =
          glm::proj(core.getDirFwd (), triUnityDirPlane1)+
          glm::proj(core.getDirFwd (), triUnityDirPlane2);
      newDirFwd /= glm::length(newDirFwd);

      // === hemisphere of up direction remains unchanged ===
      glm::vec3 newDirUp = triUnityDirNorm;
      if (glm::dot(triUnityDirNorm, core.getDirUp()) < 0.0f){
        newDirFwd *= -1.0f;
        newDirUp *= -1.0f;
      }

      glm::vec3 newPos = v0 + isBary[0] * (v1 - v0) + isBary[1] * (v2 - v0);
      core.setPosFwdUp (newPos, newDirFwd, newDirUp);
      this->trackedTri = t;

      return true;
    }
    return false;
  }

// moves a distance dist in the direction of dirFwd (projected onto map).
// returns new dirFwd and dirUp
  void track(glm::vec3& dirFwd, glm::vec3& dirUp, float dist){

  }
protected:
  terrTriDomain *ttd;
  terrTri *trackedTri;
}
;
