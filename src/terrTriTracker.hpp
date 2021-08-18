#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>

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
    terrTri *t = this->ttd->locateTriByVerticalProjection (core.getPos());
    if (t) {
      glm::vec3 v0 = t->getV0 (this->ttd);
      glm::vec3 v1 = t->getV1 (this->ttd);
      glm::vec3 v2 = t->getV2 (this->ttd);
      const glm::vec3 dirUpDown = glm::vec3 (0.0f, 1.0f, 0.0f);
      glm::vec2 isBary;
      float d;
      if (glm::intersectRayTriangle (core.getPos(), dirUpDown, v0, v1, v2, isBary, d)) {
        this->trackedTri = t;
        glm::vec3 newPos = v0 + isBary[0] * (v1 - v0) + isBary[1] * (v2 - v0);
        core.setPos(newPos);
        return true;
      } else {
        assert(0);
      }
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
