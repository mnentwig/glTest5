#pragma once
#include <glm/vec3.hpp>
class terrTriDomain;
typedef unsigned int terrTriVertIx;
class terrTri {
friend terrTriDomain;
public:
  terrTri(terrTriVertIx v0, terrTriVertIx v1, terrTriVertIx v2);

  /** neighbor n has common points pt1 and pt2 */
  void registerNeighbor(terrTri* n, terrTriVertIx pt1, terrTriVertIx pt2);
  terrTriVertIx getV0() const;
  terrTriVertIx getV1() const;
  terrTriVertIx getV2() const;
  glm::vec3 getV0(terrTriDomain* ttd) const;
  glm::vec3 getV1(terrTriDomain* ttd) const;
  glm::vec3 getV2(terrTriDomain* ttd) const;
  protected:
  terrTriVertIx v0;
  terrTriVertIx v1;
  terrTriVertIx v2;
  terrTri *n01;
  terrTri *n12;
  terrTri *n20;
};
