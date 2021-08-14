#pragma once
#include <mutex>

class GLFWwindow;
class fpvInput{
 public:
  double time_s;
  double deltaTime_s;
  bool key_forw;
  bool key_backw;
  bool key_strafeLeft;
  bool key_strafeRight;
  bool key_up;
  bool key_down;
  bool key_shift;
  bool key_cameraYawLeft;
  bool key_cameraYawRight;
  bool key_cameraPitchUp;
  bool key_cameraPitchDown;
  bool key_cameraRollLeft;  
  bool key_cameraRollRight;
  double mouseDeltaX;
  double mouseDeltaY;
  bool rightButtonChangeDown;
  bool quitApplication;
};

class fpvInputProcessor{
 public:
  fpvInputProcessor(GLFWwindow* window);
  fpvInput run();
  void mouseCallback(double mouseX, double mouseY);
  void mouseButtonCallback(int button, bool state);
protected:
  bool startup;
  double lastTime_s;  
  GLFWwindow* window;
  volatile unsigned int mouseCount;
  unsigned int lastMouseCount;
  volatile double mouseX;
  double lastMouseX;
  volatile double mouseY;
  double lastMouseY;

  std::mutex mouseMutex;
  class cMouseState{
  public:
    double x;
    double y;
    unsigned int count;
  };
  cMouseState mouseState;
  cMouseState lastMouseState;
  bool rightButtonChangeDown;
};
