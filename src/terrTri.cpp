#include <cstdlib>
#include <vector>
#include <cassert>
#include <stdexcept>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/projection.hpp>
#include <iostream>

#include "terrTri.h"
#include "terrTriDomain.h"
#include "myGl.h"

terrTri::terrTri(terrTriVertIx v0, terrTriVertIx v1, terrTriVertIx v2){
  this->v0 = v0;
  this->v1 = v1;
  this->v2 = v2;
  this->n01 = NULL;
  this->n12 = NULL;
  this->n20 = NULL;
}

void terrTri::registerNeighbor(terrTri* n, terrTriVertIx ptA, terrTriVertIx ptB){
  assert(ptA != ptB);

  bool ptA_is_v0 = (ptA == this->v0);
  bool ptB_is_v1 = (ptB == this->v1);

  if (ptA_is_v0 && ptB_is_v1) {
    assert(this->n01 == NULL);
    this->n01 == n;
    return;
  }

  bool ptB_is_v0 = (ptB == this->v0);
  bool ptA_is_v1 = (ptA == this->v1);

  if (ptB_is_v1 && ptA_is_v0) {
    assert(this->n01 == NULL);
    this->n01 == n;
    return;
  }

  bool ptB_is_v2 = (ptB == this->v2);

  if (ptA_is_v1 && ptB_is_v2) {
    assert(this->n12 == NULL);
    this->n12 == n;
    return;
  }

  bool ptA_is_v2 = (ptA == this->v2);

  if (ptA_is_v2 && ptB_is_v1) {
    assert(this->n12 == NULL);
    this->n12 == n;
    return;
  }

  if (ptA_is_v2 && ptB_is_v0) {
    assert(this->n20 == NULL);
    this->n20 == n;
    return;
  }

  if (ptA_is_v0 && ptB_is_v2) {
    assert(this->n20 == NULL);
    this->n20 == n;
    return;
  }

  throw new std::runtime_error ("tri is not neighbor (note: need bitwise identical vertices)");
}

const glm::vec3& terrTri::getV0(terrTriDomain* ttd) const{
  return ttd->getVertex(this->v0);
}

const glm::vec3& terrTri::getV1(terrTriDomain* ttd) const{
  return ttd->getVertex(this->v1);
}

const glm::vec3& terrTri::getV2(terrTriDomain* ttd) const{
  return ttd->getVertex(this->v2);
}

void terrTri::getV012(terrTriDomain* ttd, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) const{
  v0 = ttd->getVertex(this->v0);
  v1 = ttd->getVertex(this->v1);
  v2 = ttd->getVertex(this->v2);
}

terrTri* terrTri::getNeighbor01() const{
  return this->n01;
}

terrTri* terrTri::getNeighbor12() const{
  return this->n12;
}

terrTri* terrTri::getNeighbor20() const{
  return this->n20;
}

void terrTri::getAxes(terrTriDomain* ttd, glm::vec3& dirPlane1, glm::vec3& dirPlane2, glm::vec3& dirNorm){
  // === first plane vector: v0->v1 ==
  glm::vec3 edge1 = this->getV1(ttd)-this->getV0(ttd);
  glm::vec3 edge2 = this->getV2(ttd)-this->getV1(ttd);

  dirPlane1 = edge1;
  dirPlane2 = edge2-glm::proj(edge2, edge1); // vector "rejection" = original minus projection

  dirPlane1 /= glm::length(dirPlane1);
  dirPlane2 /= glm::length(dirPlane2);
  dirNorm = glm::cross(dirPlane1, dirPlane2);

  // === check for unity vectors ===
  const float eps1 = 1e-6f;
  assert(glm::length(dirNorm)-1.0f < eps1);
  assert(glm::length(dirPlane1)-1.0f < eps1);
  assert(glm::length(dirPlane2)-1.0f < eps1);

  // === check for orthogonality ===
  const float eps2 = 1e-6f;
  assert(std::fabs(glm::dot(dirNorm, dirPlane1)) < eps2);
  assert(std::fabs(glm::dot(dirPlane1, dirPlane2)) < eps2);
  assert(std::fabs(glm::dot(dirPlane2, dirNorm)) < eps2);
}

