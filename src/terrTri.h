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
  const glm::vec3& getV0(terrTriDomain* ttd) const;
  const glm::vec3& getV1(terrTriDomain* ttd) const;
  const glm::vec3& getV2(terrTriDomain* ttd) const;
  void getV012(terrTriDomain* ttd, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2) const;
  terrTri* getNeighbor01() const;
  terrTri* getNeighbor12() const;
  terrTri* getNeighbor20() const;
  //** returns orthogonal unity vectors describing the plane and its normal */
  void getAxes(terrTriDomain* ttd, glm::vec3& dirPlane1, glm::vec3& dirPlane2, glm::vec3& dirNorm);
  glm::vec3 getCog(terrTriDomain* ttd);
protected:
  terrTriVertIx v0;
  terrTriVertIx v1;
  terrTriVertIx v2;
  public:
  terrTri *n01;
  terrTri *n12;
  terrTri *n20;
};
