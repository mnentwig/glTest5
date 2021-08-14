#include "explosible.h"
#include "instStackTriInst.h"
#include "outliner.hpp"
#include "instMan.h"
#include "explTraj.h"
#include <math.h>
#include <vector>
#include <assert.h>
#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>

/** One surface of an "explosible" object that may fly away, spinning wildly
 *  Dual purpose: Hitscan detection
 */
class fragment {
public:
  fragment (unsigned int startIxTriOutline, unsigned int startIxTriFill) {
    this->startIxTriOutline = startIxTriOutline;
    this->endIxTriOutline = startIxTriOutline;
    this->startIxTriFill = startIxTriFill;
    this->endIxTriFill = endIxTriFill;
    this->vertices = new std::vector<glm::vec3> ();
  }

  void addVertex (glm::vec3& v) {
    assert(!this->closed);
    this->vertices->push_back (v);
  }

  void close (unsigned int endIxTriOutline, unsigned int endIxTriFill) {
    assert(!this->closed);
    this->endIxTriOutline = endIxTriOutline;
    this->endIxTriFill = endIxTriFill;

// === calculate center-of-gravity ===
    this->cog = glm::vec3 (0, 0, 0);
    for (unsigned int ix = 0; ix < this->vertices->size (); ++ix) {
      this->cog += (*this->vertices)[ix];
    }
    this->cog /= (float) this->vertices->size ();
    this->closed = true;

// === create collision detect triangles ===
    typedef struct {
      glm::vec3 vertex;
      float distToCenter;
    } earClipVertex;

    std::vector<earClipVertex> vList;
    for (unsigned int ix = 0; ix < this->vertices->size (); ++ix) {
      earClipVertex v;
      v.vertex = (*this->vertices)[ix];
      v.distToCenter = glm::distance2 (v.vertex, this->cog);
      vList.push_back (v);
    }

// === clip ears ===
    while (vList.size () > 2) {
// === locate outermost vertex ===
      int ixMax = 0;
      for (unsigned int ix = 1; ix < vList.size (); ++ix)
        if (vList[ix].distToCenter > vList[ixMax].distToCenter)
          ixMax = ix;

// === get triangle where ixMax identifies the outermost point ===
      int ixPrev = (ixMax - 1 + vList.size ()) % vList.size ();
      int ixNext = (ixMax + 1) % vList.size ();

// === add to collision detect triangles ===
      collisionTri t;
      t.p0 = vList[ixPrev].vertex;
      t.p1 = vList[ixMax].vertex;
      t.p2 = vList[ixNext].vertex;
      this->collisionTriList.push_back (t);

// == clip ear ===
      vList.erase (vList.begin () + ixMax);
    }
  }

  /** returns (approximate?) center-of-gravity */
  glm::vec3 getCog () const {
    return this->cog;
  }
  /** returns translation matrix to move centered object back to its original location */
  glm::mat4 getCenterCog () const {
    return glm::translate (glm::mat4 (1.0f), -this->cog);
  }
  /** returns translation matrix to move object's COG to coordinate origin for rotation */
  glm::mat4 getUncenterCog () const {
    return glm::translate (glm::mat4 (1.0f), this->cog);
  }

  /** returns a rotation axis for the part spinning while flying away */
  glm::vec3 getAxis () const {
    glm::vec3 r = (*this->vertices)[0] - this->cog;
    return glm::normalize (r);
  }

  static glm::vec3 matMul(const glm::mat4& m, const glm::vec3& v){
    return glm::vec3(m*glm::vec4(v, 1.0f));
  }

  float lineIntersectAtDistance (const glm::mat4& proj, const glm::vec3& lineOrig, const glm::vec3& lineDir) {
    float dist = glm::length (lineDir);
    for (unsigned int ix = 0; ix < this->collisionTriList.size (); ++ix) {
      collisionTri t = this->collisionTriList[ix];
      glm::vec2 isBary;
      GLfloat d;
      if (glm::intersectRayTriangle(lineOrig, lineDir,
                                    matMul(proj, t.p0), matMul(proj, t.p1), matMul(proj, t.p2), isBary, d)) {
        if ((d >= 0) && (d < dist))
          return d;
      }
    }
    return NAN;
  }

  bool lineIntersectCheck(const glm::mat4& proj, const glm::vec3& lineOrig, const glm::vec3& lineDir, float &distLimit) {
    float dist = this->lineIntersectAtDistance (proj, lineOrig, lineDir);
    if (isnan(dist))
      return false;// no intersection
    if (dist > distLimit)
      return false;// intersection but further away than current limit
    distLimit = dist;// update return value to smaller, new distance
    return true;
  }

  unsigned int startIxTriOutline;
  unsigned int startIxTriFill;
  unsigned int endIxTriOutline;
  unsigned int endIxTriFill;
protected:
  std::vector<glm::vec3> *vertices;
  glm::vec3 cog;
  bool closed = false;

  typedef struct {
  public:
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
  } collisionTri;

  std::vector<collisionTri> collisionTriList;
};

explosible::explosible (instMan *im) {
  this->im = im;
  this->imHandle = im->openHandle ();
  this->isOutline = this->im->getIsOutline (this->imHandle);
  this->isFill = this->im->getIsFill (this->imHandle);
  this->currentFragment = new fragment (this->isOutline->getTriCount (),
                                        this->isFill->getTriCount ());
}

void explosible::generateOutlinedShape (glm::vec3 *vertices, unsigned int nVertices, float width) {
  outliner::generateOutlinedShape (vertices, nVertices, width, this->isOutline, this->isFill);
  for (unsigned int ix = 0; ix < nVertices; ++ix)
    this->currentFragment->addVertex (vertices[ix]);
  this->closeFragment ();
}

void explosible::generateOutlinedBody (glm::vec3 *vertices1,
                                       glm::vec3 *vertices2,
                                       unsigned int nVertices, float width) {
  glm::vec3 pts[4];
  for (unsigned int ix1 = 0; ix1 < nVertices; ++ix1) {
    unsigned int ix2 = (ix1 + 1) % nVertices;
    pts[0] = vertices1[ix1];
    pts[1] = vertices1[ix2];
    pts[2] = vertices2[ix2];
    pts[3] = vertices2[ix1];
    outliner::generateOutlinedShape (pts, /*nVertices*/4, width,
                                     this->isOutline, this->isFill);
    this->currentFragment->addVertex (pts[0]);
    this->currentFragment->addVertex (pts[1]);
    this->currentFragment->addVertex (pts[2]);
    this->currentFragment->addVertex (pts[3]);
    this->closeFragment ();
  }
}

void explosible::closeFragment () {
  this->currentFragment->close (this->isOutline->getTriCount (), this->isFill->getTriCount ());
  this->fragments.push_back (this->currentFragment);
  this->currentFragment = new fragment (this->isOutline->getTriCount (), this->isFill->getTriCount ());
}

void explosible::finalize () {
  this->isOutline->finalize ();
  this->isFill->finalize ();
}

void explosible::render (const glm::mat4 &proj, const glm::vec3 &rgbOuter,
                         const glm::vec3 &rgbInner) {
  this->im->renderInst (this->imHandle, proj, rgbOuter, rgbInner);
}

void explosible::renderExplosion (const glm::mat4 &model2screen,
                                  const glm::mat4 &model2model,
                                  const explTraj &traj,
                                  const glm::vec3 &rgbOuter,
                                  const glm::vec3 &rgbInner) {
  for (unsigned int ix = 0; ix < this->fragments.size (); ++ix) {
    fragment *f = this->fragments[ix];
    glm::mat4 model2model_expl = glm::translate (glm::mat4 (1.0f),
                                                 traj.getDeltaPos (ix));
    glm::mat4 tot = model2screen;
    tot *= model2model_expl;
    tot *= model2model;
    tot *= f->getUncenterCog ();
    tot *= traj.getRotation (ix);
    tot *= f->getCenterCog ();
    this->isOutline->run1 (f->startIxTriOutline, f->endIxTriOutline, tot,
                           rgbOuter);
    this->isFill->run1 (f->startIxTriFill, f->endIxTriFill, tot, rgbInner);
  }
}

void explosible::explode (explTraj *traj, glm::vec3 impact, float speed,
                          float angSpeed) {
  traj->clear ();
  for (unsigned int ix = 0; ix < this->fragments.size (); ++ix) {
    glm::vec3 dir = this->fragments[ix]->getCog () - impact;
    dir = glm::normalize (dir);
    glm::vec3 axis = this->fragments[ix]->getAxis ();
    traj->registerFragment (dir, speed, axis, angSpeed);
  }
}

bool explosible::lineIntersectCheck (const glm::mat4& transf, const glm::vec3 &lineOrigin,
                                       const glm::vec3 &lineDir,
                                       float &distLimit) const {
  bool retVal = false;
  for (unsigned int ix = 0; ix < this->fragments.size (); ++ix) {
    retVal |= this->fragments[ix]->lineIntersectCheck(transf, lineOrigin, lineDir, distLimit);
  }
  return retVal;
}
