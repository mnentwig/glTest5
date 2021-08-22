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

  if (ptB_is_v0 && ptA_is_v1) {
    assert(this->n01 == NULL);
    this->n01 == n;
    return;
  }

  bool ptB_is_v2 = (ptB == this->v2);

  if (ptA_is_v1 && ptB_is_v2) {
    assert(this->n12 == NULL);
    this->n12 == n; assert(0);
    return;
  }

  bool ptA_is_v2 = (ptA == this->v2);

  if (ptB_is_v1 && ptA_is_v2) {
    assert(this->n12 == NULL);
    this->n12 == n; // assert(0);
    return;
  }

  if (ptA_is_v2 && ptB_is_v0) {
    assert(this->n20 == NULL);
    this->n20 == n;
    return;
  }

  if (ptB_is_v2 && ptA_is_v0) {
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
//  std::cout << this->n01 << " " << this->n12 << " " << this->n20 << std::endl;
  return this->n01;
}

terrTri* terrTri::getNeighbor12() const{
  //std::cout << this->n01 << " " << this->n12 << " " << this->n20 <<std::endl;
  return this->n12;
}

terrTri* terrTri::getNeighbor20() const{
  std::cout << this->n01 << " " << this->n12 << " " << this->n20 <<std::endl;
  return this->n20;
}

