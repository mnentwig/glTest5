#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>

#include "terrTri.h"
#include "terrTriPlus.hpp"
#include "terrTriDomain.h"
#include "mobNormalInterpolator.hpp"
/** instance is associated with one mob instance. Manages movement along the surface of a terrTriDomain */
class terrTriTracker {
public:
  terrTriTracker(terrTriDomain* ttd, posRot* core) : tTracked (NULL, NULL){
    this->ttd = ttd;
    this->core = core;
  }

  void setMobNormalInterpolator(mobNormalInterpolator* mni){
    this->mni = mni;
  }

  /** initializes terrTriTracker with point p that gets projected on surface.
   * Adjusts inclination of core.
   * returns true if successful (p becomes projected point) or false otherwise (p remains unchanged) */
  bool drop(){
    glm::vec3 origPos = this->core->getPos ();

// === fast intersection in 2D ===
    terrTri *t = this->ttd->locateTriByVerticalProjection (origPos);
    if (t) {
      this->tTracked = terrTriPlus (t, this->ttd);

// === setup triangle pts ===
      glm::vec3 v0;
      glm::vec3 v1;
      glm::vec3 v2;
      t->getV012 (ttd, v0, v1, v2);
      const glm::vec3 dirUpDown = glm::vec3 (0.0f, 1.0f, 0.0f);

// === intersect in 3D ===
      glm::vec2 isBary;
      float d;
      bool intersectSuccess = glm::intersectRayTriangle (origPos, dirUpDown, v0, v1, v2, isBary, d);
      assert(intersectSuccess);
      glm::vec3 newPos = v0 + isBary[0] * (v1 - v0) + isBary[1] * (v2 - v0);

      this->core->setPos (newPos);
      this->tTracked.align (this->core);
      return true;
    }
    return false;
  }

// TODO split xy functions into geomUtils2d.hpp
// TODO keep copy of core instead of param
// TODO make wrapper class for terrTri that caches constZ matrices etc
// TODO create projectMobVector() in terrTri, move out here

  /** moves a distance dist (can be negative) in the direction of dirFwd (projected onto map).
   * returns true if moved to a new triangle.
   * Interpolated normal is NOT updated*/
  bool track(float& dist){
    bool movedToNewTri = false;
    while (dist > 0.0f) {
      terrTri *newTri = this->tTracked.track (this->core, dist);
      if (newTri != NULL) {
        this->tTracked = terrTriPlus (newTri, this->ttd);
        this->tTracked.align (this->core);
        movedToNewTri = true;
      }
    }
    return movedToNewTri;
  }

  void updateInterpolatedNormal(float now_s){
    glm::vec3 finalNormal = this->core->getDirUp ();
    this->mni->set (finalNormal, now_s);
  }

protected:
  //* triangle mesh we're operating on */
  terrTriDomain *ttd;

//** location / orientation of the MOB being managed */
  posRot *core;

//** independent normal (optional) */
  mobNormalInterpolator *mni = NULL;

//* known tri being tracked */
  terrTriPlus tTracked;

//*** rotation matrix for tri plane: 2d xy back to 3d */
  glm::mat3 constZToThreeD;
  //*** rotation matrix for tri plane: 3d to 2d xyz */
  glm::mat3 threeDToConstZ;
};
