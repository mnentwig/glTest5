#pragma once
#include <map>
#include <glm/vec3.hpp>
#include <vector>
#include "terrTri.h"
/** manages adjacency relation of triangles forming a plain e.g. terrain. Note: relies on common vertexes being bitwise identical */
class terrTriDomain {
public:
  void setVertex(terrTriVertIx index, const glm::vec3& pt);
  glm::vec3 getVertex(terrTriVertIx index);
  void registerTri(terrTriVertIx p0, terrTriVertIx p1, terrTriVertIx p2);
  void reserveVertexSpace(unsigned int n);
  ~terrTriDomain();
  /** moves along a surface of terrTris.
   *
   * @param knownLastTri terrTri on which surface position is located. Will be updated if moving to other tri. NULL for startup.
   * @param position input and output: Start/end of movement vector
   * @param dirFwd expects unity vector. Movement in this direction. Will be updated (projected on triangle)
   * @param dirUp expects unity vector. "Up" direction for projection of position on triangle. Will be updated to tri normal.
   * @param dist Travel distance
   */
  void motion(terrTri** knownLastTri, glm::vec3& position, glm::vec3& dirFwd, glm::vec3& dirUp, float dist);
  terrTri* locateTriByVerticalProjection(const glm::vec3& pos);

protected:
  std::vector<glm::vec3> vertices;

  std::vector<terrTri*>* getAllTrisUsingPt(terrTriVertIx pt);
  std::vector<std::vector<terrTri*>*> trisUsingVertex;
  std::vector<terrTri*> allTerrTris;
};
