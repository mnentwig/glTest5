#include "terrTriDomain.h"
#include "terrTri.h"
#include <vector>
#include <algorithm>
#include <glm/vec2.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>

void terrTriDomain::reserveVertexSpace(unsigned int n){
  if (n > this->vertices.size ())
  this->vertices.resize (n);
  this->trisUsingVertex.resize (n);
}

void terrTriDomain::setVertex(terrTriVertIx index, const glm::vec3& pt){
  assert(index < this->vertices.size ());
  this->vertices[index] = pt;
  this->trisUsingVertex[index] = new std::vector<terrTri*> ();
}

glm::vec3 terrTriDomain::getVertex(terrTriVertIx index){
  return this->vertices[index];
}

void terrTriDomain::registerTri(terrTriVertIx p0, terrTriVertIx p1, terrTriVertIx p2){
  terrTri *t = new terrTri (p0, p1, p2);
  this->allTerrTris.push_back (t);
  std::vector<terrTri*> *n0 = this->trisUsingVertex[p0];
  std::vector<terrTri*> *n1 = this->trisUsingVertex[p1];
  std::vector<terrTri*> *n2 = this->trisUsingVertex[p2];

  for (auto it = n0->begin (); it != n0->end (); ++it) {
    if (std::find (n1->begin (), n1->end (), *it) != n1->end ()) {
      t->n01 = *it;
      (*it)->registerNeighbor (t, p0, p1);
      break;
    }
  }

  for (auto it = n1->begin (); it != n1->end (); ++it) {
    if (std::find (n2->begin (), n2->end (), *it) != n2->end ()) {
      t->n12 = *it;
      (*it)->registerNeighbor (t, p1, p2);
      break;
    }
  }

  for (auto it = n2->begin (); it != n2->end (); ++it) {
    if (std::find (n0->begin (), n0->end (), *it) != n0->end ()) {
      t->n20 = *it;
      (*it)->registerNeighbor (t, p2, p0);
      break;
    }
  }

// === once neighbor search is complete, add tri to lookup-by-vertex table ===
  n0->push_back (t);
  n1->push_back (t);
  n2->push_back (t);
}

terrTriDomain::~terrTriDomain()
{
  auto it = this->allTerrTris.begin ();
  while (it != this->allTerrTris.end ()) {
    delete (*it);
    ++it;
  }
}

void terrTriDomain::motion(terrTri** knownLastTri, glm::vec3& position, glm::vec3& dirFwd, glm::vec3& dirUp, float dist){
// === initialize ===
  if (*knownLastTri == NULL) {
    auto it = this->allTerrTris.begin ();
    while (it != this->allTerrTris.end ()) {
      terrTri *t = *(it++);
      glm::vec2 isBary;
      float d;
      glm::vec3 v0 = this->getVertex (t->getV0 ());
      glm::vec3 v1 = this->getVertex (t->getV1 ());
      glm::vec3 v2 = this->getVertex (t->getV2 ());
      if (glm::intersectRayTriangle (position, dirUp, v0, v1, v2, isBary, d)) {
        *knownLastTri = t;
        position = v0 + isBary[0] * (v1 - v0) + isBary[1] * (v2 - v0);
        position += dirUp;
//        printf("%f %f\n", (double)isBary.x, (double)isBary.y);
        break;
      }
    }
  }
}
