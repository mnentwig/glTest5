#include "terrTriDomain.h"
#include "terrTri.h"
#include <vector>
#include <algorithm> // std::find
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

const glm::vec3& terrTriDomain::getVertex(terrTriVertIx index){
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

// see https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
static float triCheckSign (const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
    return (p1.x - p3.x) * (p2.z - p3.z) - (p2.x - p3.x) * (p1.z - p3.z);
}

//** checks whether pt lies in triangle v1, v2, v3 in the xz plane. The y dimension is disregarded (dropping pt onto triangle) /*
static bool pointInTriangle (const glm::vec3& pt, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3){
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = triCheckSign(pt, v1, v2);
    d2 = triCheckSign(pt, v2, v3);
    d3 = triCheckSign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

terrTri* terrTriDomain::locateTriByVerticalProjection(const glm::vec3& pos){
  auto it = this->allTerrTris.begin ();
  while (it != this->allTerrTris.end ()) {
    terrTri *t = *(it++);
    if (pointInTriangle(pos, t->getV0(this), t->getV1(this), t->getV2(this))){
      return t;
    }
  }
  return NULL;
}

void terrTriDomain::motion(terrTri** knownLastTri, glm::vec3& position, glm::vec3& dirFwd, glm::vec3& dirUp, float dist){
// === initialize ===
  if (*knownLastTri == NULL) {
    auto it = this->allTerrTris.begin ();
    while (it != this->allTerrTris.end ()) {
      terrTri *t = *(it++);
      glm::vec2 isBary;
      float d;
      glm::vec3 v0 = t->getV0 (this);
      glm::vec3 v1 = t->getV1 (this);
      glm::vec3 v2 = t->getV2 (this);
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
