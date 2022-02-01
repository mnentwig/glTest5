#define GLFW_INCLUDE_NONE
#ifndef linux
// Windows version uses glew as it comes with its own copy of relevant headers
// see also main.cpp (requires runtime initialization)
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#else
// load GLFW for windows system support but without including any openGl headers
#include <GLFW/glfw3.h>
// include openGLES headers ourselves
#include <GLES3/gl31.h>
#endif
#define GL_CHK(x) {x;                                                   \
  GLenum glError = glGetError();                                        \
  if(glError != GL_NO_ERROR) {                                          \
    fprintf(stderr, "glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__); \
    glfwTerminate();                                                    \
    exit(1);                                                            \
  }                                                                     \
}
