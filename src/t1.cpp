#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/closest_point.hpp>

#include "myGl.h"
#include "t1.h"
#include "outliner.hpp"
#include "instMan.h"
#include "explosible.h"
#include "utils.h"
instMan *t1::im = NULL;
explosible *t1::eBody;
explosible *t1::eTurret;
explosible *t1::eGun;
float t1::hitscanPrecheckDiameter;

class t1params {
  friend t1;
  struct {
    float length;
    float nose;
    float rear;
    float width;
    float height;
  } body;
  struct {
    int nEdges;
    float radiusBottom;
    float radiusTop;
    float height;
  } turret;
  struct {
    int nEdges;
    float radius;
    float length;
  } gun;
};

static glm::vec3 polygonPt(int pos, int nPts, float radius, float y,
                           float phiOffset = 0){
  float phi = 2 * M_PI * pos / nPts + phiOffset;
  return glm::vec3 (cos (phi) * radius, y, -sin (phi) * radius);
}

void t1::createBody(instMan *im, float width, t1params *p){
  explosible *ex = new explosible (im);
  t1::eBody = ex;

// === side of body ===
  std::vector<glm::vec3> vv;
  float a = p->body.length / 2.0f - p->body.nose;
  float b = p->body.length / 2.0f;
  float c = -p->body.length / 2.0f + p->body.rear;
  float d = -p->body.length / 2.0f;
  float e = p->body.height / 2.0f;
  float f = p->body.height;
  float g = p->body.width / 2.0f;
  vv.push_back (glm::vec3 (a, 0, 0));
  vv.push_back (glm::vec3 (b, e, 0));
  vv.push_back (glm::vec3 (a, f, 0));
  vv.push_back (glm::vec3 (c, f, 0));
  vv.push_back (glm::vec3 (d, e, 0));
  vv.push_back (glm::vec3 (c, 0, 0));

  glm::mat4 m1 = glm::translate (glm::mat4 (1.0f), glm::vec3 (0.0f, 0.0f, -g));
  glm::mat4 m2 = glm::translate (glm::mat4 (1.0f), glm::vec3 (0.0f, 0.0f, g));

  std::vector<glm::vec3> side1 = myGlTransformVec (vv, m1);
  std::vector<glm::vec3> side2 = myGlTransformVec (vv, m2);
  unsigned int nV = side1.size ();
  assert(side2.size () == nV);

  ex->generateOutlinedShape (side1.data (), nV, width);
  ex->generateOutlinedShape (side2.data (), nV, width);
  ex->generateOutlinedBody (side1.data (), side2.data (), nV, width);
  ex->finalize ();
}

void t1::createTurret(instMan *im, float width, t1params *par){
  explosible *e = new explosible (im);
  t1::eTurret = e;

  unsigned int nV = par->turret.nEdges;
  std::vector<glm::vec3> vTop;
  std::vector<glm::vec3> vBottom;

  float a = par->body.height + par->turret.height;
  float b = par->body.height * 1.01;
  float c = par->turret.radiusTop;
  float d = par->turret.radiusBottom;
  for (unsigned int ix = 0; ix < nV; ++ix) {
    vTop.push_back (polygonPt (ix, nV, /*radius*/c, /*y*/a));
    vBottom.push_back (polygonPt (ix, nV, /*radius*/d, /*y*/b));
  }

  e->generateOutlinedShape (vTop.data (), nV, width);
  e->generateOutlinedShape (vBottom.data (), nV, width);
  e->generateOutlinedBody (vTop.data (), vBottom.data (), nV, width);
  e->finalize ();
}

static glm::vec3 matMul(glm::mat4 m, glm::vec3 v){
  return glm::vec3 (m * glm::vec4 (v, 1.0f));
}

void t1::createGun(instMan *im, float width, t1params *par){
  explosible *e = new explosible (im);
  t1::eGun = e;

  unsigned int nV = par->gun.nEdges;
  std::vector<glm::vec3> vFront;
  std::vector<glm::vec3> vRear;

  float a = par->body.height + par->turret.height / 2.0f;
  glm::mat4 m1 = glm::translate (glm::mat4 (1.0f),
                                 glm::vec3 (par->turret.radiusTop, a, 0.0f));
  float phi = -90.0f / 180.0f * M_PI;
  glm::mat4 rot = glm::rotate (glm::mat4 (1.0f), phi, glm::vec3 (0, 0, 1));
  m1 *= rot;
  for (unsigned int ix = 0; ix < nV; ++ix) {
    vRear.push_back (
                     matMul (
                             m1,
                             polygonPt (ix, nV, par->gun.radius, /*y*/0, /*phi_offset*/M_PI)));
    vFront.push_back (
                      matMul (m1, polygonPt (ix, nV, par->gun.radius, /*y*/par->gun.length, /*phi_offset*/
                                             M_PI)));
  }

  e->generateOutlinedBody (vFront.data (), vRear.data (), nV, width);
  e->finalize ();
}

void t1::startup(instMan *im){
  assert(t1::im == NULL);
  t1::im = im;

  t1params p;
  p.body.length = 10;
  p.body.width = 4;
  p.body.nose = 2;
  p.body.height = 2;
  p.body.rear = 1;
  p.body.height = 2;
  p.turret.nEdges = 5;
  p.turret.radiusTop = 1.0;
  p.turret.radiusBottom = 1.8;
  p.turret.height = 1;
  p.gun.nEdges = 3;
  p.gun.radius = 0.2;
  p.gun.length = 4;
  float width = 0.03f;
  createBody (im, width, &p);
  createTurret (im, width, &p);
  createGun (im, width, &p);
  t1::hitscanPrecheckDiameter = 12;
}

t1::t1(glm::vec3& pos, glm::vec3& dirFwd, glm::vec3& dirUp, glm::vec3 rgbOuter, glm::vec3 rgbInner,
       glm::vec3 rgbOuterSelected,
       glm::vec3 rgbInnerSelected) : core(pos, dirFwd, dirUp){
  this->rgbOuter = rgbOuter;
  this->rgbInner = rgbInner;
  this->rgbOuterSelected = rgbOuterSelected;
  this->rgbInnerSelected = rgbInnerSelected;
  this->lastTurretGunRot = glm::rotate (glm::mat4 (1.0f), 0.0f,
                                        glm::vec3 (0, 1, 0));
}

void t1::render(const glm::mat4 &proj, bool selected){
  glm::mat4 projT = glm::translate (proj, this->core.getPos());
//  glm::mat4 projT = this->core.model2world();
//projT = proj * projT;
  double now_s = getTime ();

  float phi = now_s * M_PI * 0.3;
  this->lastTurretGunRot = glm::rotate (glm::mat4 (1.0f), phi,
                                        glm::vec3 (0, 1, 0));

  glm::vec3 rgbO = selected ? this->rgbOuterSelected : this->rgbOuter;
  glm::vec3 rgbI = selected ? this->rgbInnerSelected : this->rgbInner;

  t1::eBody->render (projT, rgbO, rgbI);
  t1::eTurret->render (projT * this->lastTurretGunRot, rgbO, rgbI);
  t1::eGun->render (projT * this->lastTurretGunRot, rgbO, rgbI);
}

void t1::explode(glm::vec3 impact, float speed, float angSpeed){
  t1::eBody->explode (&this->trajBody, impact, speed, angSpeed);
  t1::eTurret->explode (&this->trajTurret, impact, speed, angSpeed);
  t1::eGun->explode (&this->trajGun, impact, speed, angSpeed);
}

void t1::renderExplosion(glm::mat4 world2screen){
  glm::mat4 model2world = glm::translate (glm::mat4 (1.0f), this->core.getPos()); // !!!!!!
  glm::mat4 model2screen = world2screen * model2world;

  t1::eBody->renderExplosion (model2screen, glm::mat4 (1.0f), this->trajBody,
                              this->rgbOuter,
                              this->rgbInner);
  t1::eTurret->renderExplosion (model2screen, this->lastTurretGunRot,
                                this->trajTurret,
                                this->rgbOuter,
                                this->rgbInner);
  t1::eGun->renderExplosion (model2screen, this->lastTurretGunRot,
                             this->trajGun,
                             this->rgbOuter, this->rgbInner);
}

void t1::clock(float deltaT_s){
  this->trajBody.clock (deltaT_s);
  this->trajTurret.clock (deltaT_s);
  this->trajGun.clock (deltaT_s);
}

bool t1::hitscanCheck(const glm::vec3 &lineOrigin, const glm::vec3 &lineDelta,
                        float &dist) const{
  // === pre-check (optimization) ===
  glm::vec3 lineEndpoint = lineOrigin + lineDelta;
  glm::vec3 closestPoint = glm::closestPointOnLine(this->core.getPos(), lineOrigin, lineEndpoint);
  float tmpDist = glm::distance(this->core.getPos(), closestPoint);
  #if 0
  // DEBUG: pick object closest to line
  if (tmpDist < dist){
    dist = tmpDist;
    return true;
  }
  return false;
#endif
  if (tmpDist > this->hitscanPrecheckDiameter)
    return false;
  glm::mat4 transfBody = glm::translate (glm::mat4 (1.0f), this->core.getPos());
//  glm::mat4 transfTurretGun = glm::translate (this->lastTurretGunRot, this->pos);
  glm::mat4 transfTurretGun = transfBody * this->lastTurretGunRot;
  bool retVal = this->eBody->lineIntersectCheck (transfBody, lineOrigin, lineDelta, dist);
  retVal |= this->eTurret->lineIntersectCheck (transfTurretGun, lineOrigin, lineDelta, dist);
  retVal |= this->eGun->lineIntersectCheck (transfTurretGun, lineOrigin, lineDelta, dist);
  return retVal;
}
