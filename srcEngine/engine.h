#pragma once
#include <set>
#include "fpsEstimator.hpp"

class GLFWwindow;
namespace engine {
  enum state_e {
    NEW, READY, PRE_DRAW, DRAW, SHUTDOWN
  };

  class preDrawState {
  public:
    float time_s;
    float deltaTime_s = 0;

    float mouseX;
    float deltaMouseX = 0;
    float mouseY;
    float deltaMouseY = 0;

    bool windowClose;

    long frame = 0;
  };

  class engine {
  public:
    engine();
    void startup();
    const preDrawState* preDraw();
    void beginDraw();
    void endDraw();
    void shutdown();
    /** screen width. Set before initialization, read after window resize notification */
    unsigned int screenWidth = 800;
    /** screen height. Set before initialization, read after window resize notification */
    unsigned int screenHeight = 600;

    bool testKeycodePressed(int keycode);
    bool testKeycodePressEvt(int keycode);
    bool testKeycodeReleaseEvt(int keycode);

    float fps;
    protected:
    state_e state = NEW;
    GLFWwindow *window = NULL;
    preDrawState pdsCurr;
    preDrawState pdsPrev;
    static void window_size_callback(GLFWwindow* window, int width, int height);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void keyButtonCallback(int scancode, int action);

    std::set<int> keycodeDeltaDown;
    std::set<int> keycodeDeltaUp;
    std::set<int> keycodeIsDown;

    fpsEstimator fpsEst = fpsEstimator(1.0f);
  };
// class
}// namespace
