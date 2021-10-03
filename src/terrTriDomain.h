#pragma once
#include <map>
#include <glm/vec3.hpp>
#include <vector>
#include "terrTri.h"

/** Class manages adjacency relation of triangles forming a surface. E.g. map or drivable highway.
 * The guiding assumption is that a MOB is located on one triangle in the "domain" and may move to an adjacent triangle.
 * TODO: should this be stored / recalled in binary format if building it takes too long? */
class terrTriDomain {
public:
  terrTriDomain();
  void setVertex(terrTriVertIx index, const glm::vec3& pt);
  const glm::vec3& getVertex(terrTriVertIx index) const;
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
  void debug();
  std::vector<terrTri*>* getTrisUsingVertex(terrTriVertIx pt);
//  void collectNeighbors(std::vector<terrTri*>* collection, terrTriVertIx pt) const;

  /** returns averaged normal of all tris using the vertex*/
  const glm::vec3& getVertexNormal(terrTriVertIx ix) const;

   /** close after all vertices/tris have been added to precompute vertex normals */
  void close();
  protected:
  bool state_closed;
  std::vector<glm::vec3> vertices;

  std::vector<terrTri*>* getAllTrisUsingPt(terrTriVertIx pt);
  std::vector<std::vector<terrTri*>*> trisUsingVertex;
  std::vector<terrTri*> allTerrTris;
  std::vector<glm::vec3> vertexNormals;
};
