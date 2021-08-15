#include "fpvInputProcessor.h"
#include <GLFW/glfw3.h>

fpvInputProcessor::fpvInputProcessor(GLFWwindow* window){
  this->startup = true;
  this->window = window;
  this->mouseState.count = 0;
  this->lastMouseState.count = 0;
  this->rightButtonChangeDown = false;
  this->lastMouseX = 0; // suppress warning
  this->lastMouseY = 0; // suppress warning
  this->lastMouseCount = 0; // suppress warning
  this->lastTime_s = 0; // suppress warning
}

void fpvInputProcessor::mouseCallback(double mouseX, double mouseY){
  this->mouseMutex.lock();

  this->mouseState.x = mouseX;
  this->mouseState.y = mouseY;

  ++this->mouseState.count;
  if (this->mouseState.count == 0)
    ++this->mouseState.count; // avoid 0 on wrap as it flags startup

  this->mouseMutex.unlock();
}
void fpvInputProcessor::mouseButtonCallback(int button, bool state){
  if ((button == 1) && state )
    this->rightButtonChangeDown = true;
}

fpvInput fpvInputProcessor::run(){
  fpvInput r;
  r.time_s = glfwGetTime();
  if (this->startup){
    r.deltaTime_s = 0;
  } else {
    r.deltaTime_s = r.time_s - this->lastTime_s;
  }
  this->startup = false;
  
  this->lastTime_s = r.time_s;

  r.key_strafeLeft = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
  r.key_strafeRight = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
  r.key_forw = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
  r.key_backw = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ;
  r.key_up = (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);
  r.key_down = (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
  
  r.key_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
  r.key_cameraYawLeft = (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
  r.key_cameraYawRight = (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
  r.key_cameraPitchDown = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
  r.key_cameraPitchUp = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
  r.key_cameraRollLeft = (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS);
  r.key_cameraRollRight = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);

  cMouseState currentMouseState;
  this->mouseMutex.lock();
  currentMouseState = this->mouseState;
  this->mouseMutex.unlock();

  r.mouseDeltaX = 0;
  r.mouseDeltaY = 0;
  if (currentMouseState.count != this->lastMouseState.count){
    if (this->lastMouseState.count > 0){
      r.mouseDeltaX = currentMouseState.x - this->lastMouseState.x;
      r.mouseDeltaY = currentMouseState.y - this->lastMouseState.y;
    }
    this->lastMouseState = currentMouseState;
  }
  
  r.rightButtonChangeDown = this->rightButtonChangeDown;
  this->rightButtonChangeDown = false;

  r.quitApplication = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
  r.key_F1 = (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS);
  return r;
}
