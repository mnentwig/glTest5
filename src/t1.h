#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "explTraj.h"
#include "posRot.hpp"
#include "controllable.h"
#include "terrTriTracker.hpp"
class instMan;
class t1params;
class explosible;
class terrTriDomain;
class t1: public controllable{
 public:
  static instMan* im;
  static void startup(instMan* im);
  t1(terrTriDomain* ttd, glm::vec3& pos, glm::vec3& dirFwd, glm::vec3& dirUp, glm::vec3 rgbOuter, glm::vec3 rgbInner, glm::vec3 rgbOuterSelected, glm::vec3 rgbInnerSelected);
  void render(const glm::mat4& proj, bool selected); // note: delayed via instMan
  void renderExplosion(glm::mat4 world2screen); // note: immediate
  void explode(glm::vec3 impact, float speed, float angSpeed);
  void clock(float deltaT_s);
  bool hitscanCheck(const glm::vec3 &lineOrigin, const glm::vec3 &lineDelta, float& dist) const;
  void giveInput(fpvInput inp);
  bool getSelAttempt(glm::vec3& orig, glm::vec3& dir);
  glm::mat4 getCameraView(); // implements controllable::-

  //** projects on vertical axis onto surface (down and up), updates position, dirFwd and dirUp
  void drop();
protected:  

  //** position and orientation */
  posRot core;

  terrTriTracker ttt;

  static void createBody(instMan* im, float width, t1params* par);
  static void createTurret(instMan* im, float width, t1params* par);
  static void createGun(instMan* im, float width, t1params* par);

  //** shared object template for instanced rendering */
  static explosible* eBody;

  //** shared object template for instanced rendering */
  static explosible* eTurret;

  //** shared object template for instanced rendering */
  static explosible* eGun;

  explTraj trajBody;
  explTraj trajTurret;
  explTraj trajGun;

  glm::vec3 rgbOuter;
  glm::vec3 rgbInner;
  glm::vec3 rgbOuterSelected;
  glm::vec3 rgbInnerSelected;
  glm::mat4 lastTurretGunRot;

  /** diameter of enclosing sphere to skip per-surface hitscan */
  static float hitscanPrecheckDiameter;
};
