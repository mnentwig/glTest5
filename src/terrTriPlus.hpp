#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/projection.hpp>

#include "terrTri.h"
#include "terrTriDomain.h"
#include "posRot.hpp"
#include "geomUtils2d.hpp"

/** wrapper around terrTri with cached storage. Keep only for special terrTris e.g. under MOB, not all (memory) */
class terrTriPlus {
public:
  terrTriPlus(terrTri* t, terrTriDomain* ttd){
    this->trackedTri = t;
    this->ttd = ttd;
    if (t == NULL)
    return;

// === create basis vectors ===
    glm::vec3 edge1 = this->trackedTri->getV1 (ttd) - this->trackedTri->getV0 (ttd);
    glm::vec3 edge2 = this->trackedTri->getV2 (ttd) - this->trackedTri->getV1 (ttd);

    this->dir1 = glm::normalize (edge1);
    this->dir2 = glm::normalize (edge2 - glm::proj (edge2, edge1));// vector "rejection" = original minus projection
    this->dirNorm = glm::cross (this->dir1, this->dir2);
    this->sanityCheckBasisVectors ();

//** from xy plane with constant z to original 3D space */
    this->constZToThreeD[0] = dir1;
    this->constZToThreeD[1] = dir2;
    this->constZToThreeD[2] = dirNorm;

// ** from 3D to xy with constant z => inverse of above */
    this->threeDToConstZ = glm::inverse (this->constZToThreeD);
  }

  void align(posRot* core){
    glm::vec3 prevDirFwd = core->getDirFwd ();

// TODO: use orig minus projection on normal
    glm::vec3 newDirFwd = glm::normalize (glm::proj (prevDirFwd, this->dir1) + glm::proj (prevDirFwd, this->dir2));

// === avoid spinning MOB around, placing on bottom side of tri ===
    if (glm::dot (newDirFwd, prevDirFwd) < 0.0f) {
      newDirFwd *= -1.0f;
    }

// === assign new position and inclination ===
    glm::vec3 newDirUp = this->dirNorm;
    glm::vec3 newDirLat = glm::normalize (glm::cross (newDirFwd, newDirUp));
    core->setFwdUpLat (newDirFwd, newDirUp, newDirLat);
  }

  /* moves "dist" in core.dirFwd direction. Does not change core inclination.
   * Returns neighboring terrTri if crossed or NULL if no change.
   * Moved "dist" is subtracted.
   */
  terrTri* track(posRot* core, float& dist){

    glm::vec3 v0_3d;
    glm::vec3 v1_3d;
    glm::vec3 v2_3d;

// === project triangle on constant-z plane ===
    this->trackedTri->getV012 (this->ttd, v0_3d, v1_3d, v2_3d);
    glm::vec3 v0_constZ = this->threeDToConstZ * v0_3d;
    glm::vec3 v1_constZ = this->threeDToConstZ * v1_3d;
    glm::vec3 v2_constZ = this->threeDToConstZ * v2_3d;
    glm::vec3 posStart_constZ = this->threeDToConstZ * core->getPos ();
    glm::vec3 dirFwd_constZ = this->threeDToConstZ * core->getDirFwd ();
    glm::vec3 posEnd_constZ = posStart_constZ + dirFwd_constZ * dist;
    if (geomUtils2d::pointInTriangleNoZ (posEnd_constZ, v0_constZ, v1_constZ, v2_constZ)) {
      core->move (dist * core->getDirFwd ());
      dist = 0.0f;
      return NULL;// NULL indicates no change in tri
    } else {

      std::vector<terrTri*> neighbors;
      this->trackedTri->collectNeighbors(this->ttd, &neighbors);
      // TODO three checks have many common subexpressions
      glm::vec3 intersection_constZ;
      terrTri *neighbor = NULL;
#if 0
      std::cout << "tri 2d:\n";
      glmPrint (v0_constZ);
      glmPrint (v1_constZ);
      glmPrint (v2_constZ);
      std::cout << "movement 2d:\n";
      glmPrint (posStart_constZ);
      glmPrint (posEnd_constZ);
#endif
// TODO coplanarity check for posEnd may fail if movement vector is not aligned with tri.
      geomUtils2d::assertCoplanarityZ (v0_constZ, v1_constZ, posStart_constZ, posEnd_constZ);
      if (geomUtils2d::calcLineLineIntersectionNoZ (v0_constZ, v1_constZ, posStart_constZ, posEnd_constZ, intersection_constZ)) {
        neighbor = this->trackedTri->getNeighbor01 ();
        assert(neighbor);
      } else if (geomUtils2d::calcLineLineIntersectionNoZ (v1_constZ, v2_constZ, posStart_constZ, posEnd_constZ, intersection_constZ)) {
        neighbor = this->trackedTri->getNeighbor12 ();
        assert(neighbor);
      } else if (geomUtils2d::calcLineLineIntersectionNoZ (v2_constZ, v0_constZ, posStart_constZ, posEnd_constZ, intersection_constZ)) {
        neighbor = this->trackedTri->getNeighbor20 ();
        assert(neighbor);
      } else {
#error unravel the above mess using neighbor detection via terrTri.collectNeighbors
        assert(0);
      }
// === place 2d intersection onto 2d tri z ===
      intersection_constZ.z = v0_constZ.z;
      glm::vec3 intersection_3d = this->constZToThreeD * intersection_constZ;

// hypothetical numerical precision issue because intersection is exactly on line
//      const float alpha = 1.0f;
//      intersection_3d = alpha * intersection_3d + (1.0f-alpha) * neighbor->getCog(this->ttd);
      core->setPos (intersection_3d);
      dist -= glm::distance (posStart_constZ, intersection_constZ);
      return neighbor;
    }
  }

protected:
  terrTri *trackedTri;

  /** our tri "t" is managed by this set of tris */
  terrTriDomain *ttd;

//* first vector on tri plane (dir1/dir2/dirNorm are unity and orthogonal */
  glm::vec3 dir1;

//* second vector on tri plane (dir1/dir2/dirNorm are unity and orthogonal */
  glm::vec3 dir2;

//* normal vector (dir1/dir2/dirNorm are unity and orthogonal */
  glm::vec3 dirNorm;

//** projects tri on a constant-Z plane for 2D geometry */
  glm::mat3 constZToThreeD;

//** projects 2D-plus-constant-Z back to 3D */
  glm::mat3 threeDToConstZ;

  void sanityCheckBasisVectors(){
    const float eps1 = 1e-6f;
    assert(glm::length (this->dir1) - 1.0f < eps1);
    assert(glm::length (this->dir2) - 1.0f < eps1);
    assert(glm::length (this->dirNorm) - 1.0f < eps1);

    const float eps2 = 1e-6f;
    assert(std::fabs (glm::dot (this->dirNorm, this->dir1)) < eps2);
    assert(std::fabs (glm::dot (this->dir1, this->dir2)) < eps2);
    assert(std::fabs (glm::dot (this->dir2, this->dirNorm)) < eps2);
  }
};
