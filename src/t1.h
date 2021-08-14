#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "explTraj.h"
class instMan;
class t1params;
class explosible;
class t1{
 public:
  static instMan* im;
  static void startup(instMan* im);
  t1(glm::vec3 pos, glm::vec3 rgbOuter, glm::vec3 rgbInner, glm::vec3 rgbOuterSelected, glm::vec3 rgbInnerSelected);
  void render(const glm::mat4& proj, bool selected); // note: delayed via instMan
  void renderExplosion(glm::mat4 world2screen); // note: immediate
  void explode(glm::vec3 impact, float speed, float angSpeed);
  void clock(float deltaT_s);
  bool hitscanCheck(const glm::vec3 &lineOrigin, const glm::vec3 &lineDelta, float& dist) const;
protected:  
  static void createBody(instMan* im, float width, t1params* par);
  static void createTurret(instMan* im, float width, t1params* par);
  static void createGun(instMan* im, float width, t1params* par);

  static explosible* eBody;
  static explosible* eTurret;
  static explosible* eGun;

  explTraj trajBody;
  explTraj trajTurret;
  explTraj trajGun;

  glm::vec3 pos;
  glm::vec3 rgbOuter;
  glm::vec3 rgbInner;
  glm::vec3 rgbOuterSelected;
  glm::vec3 rgbInnerSelected;
  glm::mat4 lastTurretGunRot;

  /** diameter of enclosing sphere to skip per-surface hitscan */
  static float hitscanPrecheckDiameter;
};
