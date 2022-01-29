#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include "instMan.h"
#include "explosible.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

#include "allMyGlIncludes.h"

static GLuint WIDTH = 800;
static GLuint HEIGHT = 600;

//#define SINGLE_BUFFER
static void window_size_callback(GLFWwindow* /*window*/, int width, int height){
  WIDTH = width;
  HEIGHT = height;
  glViewport (0, 0, width, height);
  std::cout << WIDTH << " " << HEIGHT << std::endl;
}

static void cursor_position_callback(GLFWwindow* /*window*/, double xpos, double ypos){
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      break;
    default:
      return;
  }

  switch (action) {
    case GLFW_PRESS:
      break;
    case GLFW_RELEASE:
      break;
    default:
      return;
  }
}

GLFWwindow* startupWindow(){
  if (!glfwInit ()) {
    fprintf ( stderr, "Failed to initialize GLFW\n");
    exit (EXIT_FAILURE);
  }

  glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef SINGLE_BUFFER
  glfwWindowHint ( GLFW_DOUBLEBUFFER, GL_FALSE);
#else
  glfwWindowHint ( GLFW_DOUBLEBUFFER, GL_TRUE);
#endif
  glfwWindowHint (GLFW_SAMPLES, 4);
// ignored for GLES glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); GLFW_CHK;

#if 0
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL); GLFW_CHK;
#else
  GLFWmonitor *monitor = glfwGetPrimaryMonitor ();
  const GLFWvidmode *mode = glfwGetVideoMode (monitor);
  glfwWindowHint (GLFW_RED_BITS, mode->redBits);
  glfwWindowHint (GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint (GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint (GLFW_REFRESH_RATE, mode->refreshRate);
  GLFWwindow *window = glfwCreateWindow (mode->width, mode->height, __FILE__, monitor, NULL);
  window_size_callback (window, mode->width, mode->height);
#endif
  glfwMakeContextCurrent (window);
  if (1 || glfwExtensionSupported ("WGL_EXT_swap_control_tear")
      || glfwExtensionSupported ("GLX_EXT_swap_control_tear")) {
    glfwSwapInterval (0);
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

explosible* generateTestcube(float edgeLen, float width, instMan* im){
  explosible *ex = new explosible (im);
  float e = 0.5*edgeLen;
  std::vector<glm::vec3> v1;
  v1.push_back (glm::vec3 (-e, -e, -e));
  v1.push_back (glm::vec3 (-e,  e, -e));
  v1.push_back (glm::vec3 ( e,  e, -e));
  v1.push_back (glm::vec3 ( e, -e, -e));
  std::vector<glm::vec3> v2;
  v2.push_back (glm::vec3 (-e, -e,  e));
  v2.push_back (glm::vec3 (-e,  e,  e));
  v2.push_back (glm::vec3 ( e,  e,  e));
  v2.push_back (glm::vec3 ( e, -e,  e));

  unsigned int nV = v1.size ();
  assert(v2.size () == nV);

  ex->generateOutlinedShape (v1.data (), nV, width);
  ex->generateOutlinedShape (v2.data (), nV, width);
  ex->generateOutlinedBody (v1.data (), v2.data (), nV, width);
  ex->finalize ();
  return ex;
}

int main(void){
  instMan im;
  srand (0);
#ifdef NDEBUG
  std::cout << "NDEBUG set - assert disabled" << std::endl;
  assert(0);
#endif

  GLFWwindow *window = startupWindow ();

  glfwSetWindowSizeCallback (window, window_size_callback);
  glfwSetCursorPosCallback (window, cursor_position_callback);
  glfwSetMouseButtonCallback (window, mouse_button_callback);
  glfwSetInputMode (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

  explosible* testcube = generateTestcube(0.8f, 0.05f, &im);

  glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glViewport (0, 0, WIDTH, HEIGHT);

  while (true) {
    glfwPollEvents ();
    if (glfwWindowShouldClose (window))
    break;
    myGlClear ();

    glm::vec3 eye(0, 5, 0);
    glm::vec3 far(0, 5, -1);
    glm::vec3 up(0, 1, 0);
    glm::mat4 view = glm::lookAt (eye, far, up);

    glm::mat4 proj = glm::perspective (45.0f, 1.0f * WIDTH / HEIGHT, 0.01f,
                                       1000.0f) * view;

    im.startFrame ();

    glm::vec3 rgbOuter(0, 1, 0);
    glm::vec3 rgbInner(0, 0, 0);
    for (float x = -100; x <= 100; ++x){
      for (float z = -100; z <= 10; ++z){
        //glm::mat4 v = glm::inverse (glm::lookAt (glm::vec3 (0, 0, 0), this->dirFwd, dirUp));
        glm::mat4 v = glm::translate (glm::mat4 (1.0f), glm::vec3(x, 0, z));//; * v;
        glm::mat4 projT = proj * v;
        testcube->render(projT, rgbOuter, rgbInner);
      }
    }

    im.endFrame ();

// === show new image ===
#ifdef SINGLE_BUFFER
    glFlush ();
#else
    glfwSwapBuffers (window);
#endif
  }
  glfwTerminate ();
  std::cout << "main Done" << std::endl;
  return EXIT_SUCCESS;
}
