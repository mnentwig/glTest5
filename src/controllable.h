#pragma once
#include "fpvInputProcessor.h"
#include <glm/vec3.hpp>
class controllable {
 public:
  controllable(){};
  virtual ~controllable(){};
  virtual void giveInput(fpvInput inp) = 0;
  /** checks, whether "giveInput" tried to select something */
  virtual bool getSelAttempt(glm::vec3 &orig, glm::vec3 &dir) = 0;
};
