#include "terrTri.h"
#include "terrTriDomain.h"
#include <cstdlib>
#include <vector>
#include <cassert>
#include <stdexcept>

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

glm::vec3 terrTri::getV0(terrTriDomain* ttd) const{
  return ttd->getVertex(this->v0);
}
glm::vec3 terrTri::getV1(terrTriDomain* ttd) const{
  return ttd->getVertex(this->v1);
}
glm::vec3 terrTri::getV2(terrTriDomain* ttd) const{
  return ttd->getVertex(this->v2);
}

terrTriVertIx terrTri::getV0() const{
  return this->v0;
}
terrTriVertIx terrTri::getV1() const{
  return this->v1;
}
terrTriVertIx terrTri::getV2() const{
  return this->v2;
}
