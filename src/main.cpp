#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <vector>

#include "myGl.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

#include "instStackTri.h"
#include "instStackLine.h"
#include "instStackTriInst.h"
#include "renderText.hpp"
#include "coordReference.hpp"
#include "outliner.hpp"
#include "instMan.h"
#include "t1.h"
#include "fpvInputProcessor.h"
#include "freeRoamCamera.h"
#include "fpsEstimator.hpp"
#include "crosshairs.h"
#include "terrain.h"
#include "terrTri.h"
volatile static GLuint WIDTH = 800;
volatile static GLuint HEIGHT = 600;

//#define SINGLE_BUFFER
static void window_size_callback(GLFWwindow* /*window*/, int width, int height){
  WIDTH = width;
  HEIGHT = height;
  glViewport (0, 0, width, height);
  std::cout << WIDTH << " " << HEIGHT << std::endl;
}

fpvInputProcessor *iProc;
static void cursor_position_callback(GLFWwindow* /*window*/, double xpos, double ypos){
  iProc->mouseCallback (xpos, ypos);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
  int reportButton;
  bool reportState;
  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      reportButton = 0;
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      reportButton = 1;
      break;
    default:
      return;
  }

  switch (action) {
    case GLFW_PRESS:
      reportState = true;
      break;
    case GLFW_RELEASE:
      reportState = false;
      break;
    default:
      return;
  }
  iProc->mouseButtonCallback (reportButton, reportState);
}

GLFWwindow* startupWindow(){
  if (!glfwInit ()) {
    fprintf ( stderr, "Failed to initialize GLFW\n");
    exit (EXIT_FAILURE);
  }

  glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  GLFW_CHK;
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  GLFW_CHK;
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
  GLFW_CHK;
#ifdef SINGLE_BUFFER
  glfwWindowHint ( GLFW_DOUBLEBUFFER, GL_FALSE);
  GLFW_CHK;
#else
  glfwWindowHint ( GLFW_DOUBLEBUFFER, GL_TRUE);
  GLFW_CHK;
#endif
  glfwWindowHint (GLFW_SAMPLES, 4);
  GLFW_CHK;
// ignored for GLES glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); GLFW_CHK;

#if 0
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL); GLFW_CHK;
#else
  GLFWmonitor *monitor = glfwGetPrimaryMonitor ();
  GLFW_CHK;
  const GLFWvidmode *mode = glfwGetVideoMode (monitor);
  GLFW_CHK;
  glfwWindowHint (GLFW_RED_BITS, mode->redBits);
  GLFW_CHK;
  glfwWindowHint (GLFW_GREEN_BITS, mode->greenBits);
  GLFW_CHK;
  glfwWindowHint (GLFW_BLUE_BITS, mode->blueBits);
  GLFW_CHK;
  glfwWindowHint (GLFW_REFRESH_RATE, mode->refreshRate);
  GLFW_CHK;
  GLFWwindow *window = glfwCreateWindow (mode->width, mode->height, __FILE__, monitor, NULL);
  window_size_callback (window, mode->width, mode->height);
  GLFW_CHK;
#endif
  glfwMakeContextCurrent (window);
  GLFW_CHK;
  if (1 || glfwExtensionSupported ("WGL_EXT_swap_control_tear")
      || glfwExtensionSupported ("GLX_EXT_swap_control_tear")) {
    glfwSwapInterval (0);
    GLFW_CHK;
  }
  return window;
}

void myGlClear(){
// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); return;

// https://www.seas.upenn.edu/~pcozzi/OpenGLInsights/OpenGLInsights-TileBasedArchitectures.pdf
  glDisable (GL_SCISSOR_TEST);
  glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask (GL_TRUE);
  glStencilMask (0xFFFFFFFF);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

int main(void){
  srand (0);
#ifdef NDEBUG
  std::cout << "NDEBUG set - assert disabled" << std::endl;
  assert(0);
#endif

  GLFWwindow *window = startupWindow ();

  iProc = new fpvInputProcessor (window);
  glfwSetWindowSizeCallback (window, window_size_callback);
  GLFW_CHK;
  glfwSetCursorPosCallback (window, cursor_position_callback);
  GLFW_CHK;
  glfwSetMouseButtonCallback (window, mouse_button_callback);
  GLFW_CHK;
  glfwSetInputMode (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  GLFW_CHK;
//if (glfwRawMouseMotionSupported())
//  glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  printf ("GL_VERSION  : %s\n", glGetString (GL_VERSION));
  printf ("GL_RENDERER : %s\n", glGetString (GL_RENDERER));

#ifndef linux
  // Windows version uses GLEW to load openGl libraries but this requires initialization
  // see also myGl.h
  glewExperimental = 1;// Needed for core profile
  if (glewInit () != GLEW_OK) {
    fprintf (stderr, "Failed to initialize GLEW\n");
    glfwTerminate ();
    exit (EXIT_FAILURE);
  }
#endif
  GL_CHK(glClearColor (0.0f, 0.0f, 0.0f, 1.0f));
  GL_CHK(glEnable (GL_DEPTH_TEST));
  GL_CHK(glEnable (GL_BLEND));
  GL_CHK(glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  GL_CHK(glViewport (0, 0, WIDTH, HEIGHT));

  glm::vec3 initPos = glm::vec3 (0, 0, -15);
  glm::vec3 initDirFwd = glm::vec3 (0, 0, 1);
  glm::vec3 initDirUp = glm::vec3 (0, 1, 0);
  freeRoamCamera *fpvCam = new freeRoamCamera (initPos, initDirFwd, initDirUp);
  fpsEstimator fpsEst (/*averaging window*/1.0f);
  terrTriDomain d;
  terrain myMap ("heightfield/heightfield.raw", d);
  instMan im;

  instStackLine *is3 = new instStackLine ();
//  renderText(is3, "The quick brown fox jumps over the lazy dog");
  glm::mat4 projText = glm::mat4 (1.0f);
  float textScale = 0.07;
  projText[0].x = textScale;
  projText[1].y = textScale;
  projText[2].z = textScale;
  projText[3].x = -1;
  projText[3].y = 1;
  is3->finalize ();

  coordReference *coordRef = new coordReference ();

  const unsigned int NT1 = 200;
  t1::startup (&im);
  t1 *tanks[NT1];
  const unsigned int divider = 15;
  for (unsigned int ix = 0; ix < NT1; ++ix) {
    unsigned int gridX = ix / divider;
    unsigned int gridY = ix - (ix / divider) * divider;
    glm::vec3 rgbOuter = glm::vec3 (1.0f, 0.0f, 0.0f);
    glm::vec3 rgbInner = glm::vec3 (0.05f, 0.0f, 0.0f);
    glm::vec3 rgbOuterSelected = glm::vec3 (1.0f, 1.0f, 1.0f);
    glm::vec3 rgbInnerSelected = glm::vec3 (0.05f, 0.0f, 0.0f);
    glm::vec3 pos (12 * gridX, 0, 6 * gridY);
    glm::vec3 dirFwd = glm::normalize (glm::vec3 (-1.0f, 0, 1.0f));
    glm::vec3 dirUp (0, 1.0f, 0);
    tanks[ix] = new t1 (pos, dirFwd, dirUp,
                        rgbOuter,
                        rgbInner,
                        rgbOuterSelected,
                        rgbInnerSelected);
  }
  tanks[0]->explode (glm::vec3 (0, 0, 0), 5.0f, 90.0f * M_PI / 180.0f);

  crosshairs::startup (&im);
  crosshairs myCrosshairs;

  float lastFpsUpdate = 0;
  t1 *selected = NULL;
  while (true) {
    glfwPollEvents ();
    if (glfwWindowShouldClose (window))
    break;
    fpvInput inputData = iProc->run ();
    if (inputData.quitApplication)
    break;
    myGlClear ();

// === detect user input, update time ===
    while (inputData.key_shift) {
      tanks[0]->explode (glm::vec3 (0, 0, 0), 5.0f, 90.0f * M_PI / 180.0f);
      inputData = iProc->run ();
      glfwPollEvents ();
    }
// === FPS display ===
    float fps = fpsEst.enterFrameGetFps (inputData.time_s);
    if (inputData.time_s > lastFpsUpdate + 1.0f) {
// printf("%2.2f FPS\n", (double)fps);
//      printf ("%3.1f ms\n", 1000.0 / (double) fps);
      lastFpsUpdate += 1.0f;
    }

// === get camera ===
    if (selected == NULL) {
      fpvCam->giveInput (inputData);
    } else {
      selected->giveInput(inputData);
    }

#if 1
    glm::vec3 testPos = fpvCam->getEye ();
    glm::vec3 testDirFwd = fpvCam->getDirFwd ();
    glm::vec3 testDirUp = fpvCam->getDirUp ();
    terrTri *movTri = NULL;
    d.motion (&movTri, testPos, testDirFwd, testDirUp, 1.0f);
    if (movTri != NULL) {
//    fpvCam->setEye (testPos);
    }
#endif
    glm::mat4 view = selected ? selected->getCameraView() : fpvCam->getCameraView ();
    glm::mat4 proj = glm::perspective (45.0f, 1.0f * WIDTH / HEIGHT, 0.01f,
                                       1000.0f) * view;

    char buf[256];
    sprintf (buf, "%2.2f FPS %i", (double) fps, movTri == NULL);
    is3->reset ();
    renderText (is3, buf, glm::vec3 (0.0, 1.0, 0.0));
    is3->finalize ();
    is3->run (projText, WIDTH, HEIGHT);

// === selection ===
    glm::vec3 selOrig, selDir;
    bool selAttempt = fpvCam->getSelAttempt (selOrig, selDir);
    if (selAttempt) {
      float selDist = INFINITY;
      selected = NULL;
      for (unsigned int ix = 0; ix < NT1; ++ix) {
        if (tanks[ix]->hitscanCheck (selOrig, selDir, selDist)) {
          selected = tanks[ix];
        }
      }
    }

// === draw coordinate reference ===
    coordRef->run (proj, WIDTH, HEIGHT);

// === draw scenery ===
    glm::vec3 colOdd (0.3f, 0.3f, 0.3f);
    glm::vec3 colEven (0.0f, 0.0, 1.0f);

    const float maxRenderDistance = 1000.0f;
    myMap.render (proj, fpvCam->getEye (), maxRenderDistance, colOdd, colEven);

    im.startFrame ();
    tanks[0]->renderExplosion (proj);
    for (unsigned int ix = 1; ix < NT1; ++ix) {
      tanks[ix]->render (proj, tanks[ix] == selected);
    }

// === crosshairs ===
    glm::mat4 Mch1 = glm::translate (glm::mat4 (1.0f), glm::vec3 (0, 0, -50));
    glm::mat4 Mch2 = glm::inverse (view);// note: straightforward but inefficient - view cancels out
    glm::mat4 Mch3 = proj * Mch2 * Mch1;
    myCrosshairs.render (Mch3);

    im.endFrame ();
    tanks[0]->clock (inputData.deltaTime_s);

// === show new image ===
#ifdef SINGLE_BUFFER
    glFlush ();
#else
    glfwSwapBuffers (window);
#endif
  }
  im.shutdown ();
  glfwTerminate ();
  return EXIT_SUCCESS;
}
