#pragma once
#include <glm/fwd.hpp>
class instMan;
class crosshairs {
public:
  static void startup(instMan *im);
  crosshairs();
  void render(glm::mat4& proj);
protected:
  static unsigned int imHandle;
  static instMan *im;
};
