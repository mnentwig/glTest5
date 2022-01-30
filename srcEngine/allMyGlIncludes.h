#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
// include openGLES headers ourselves
#include <GLES3/gl31.h>

#define GL_CHK(x) {x;                                                   \
  GLenum glError = glGetError();                                        \
  if(glError != GL_NO_ERROR) {                                          \
    fprintf(stderr, "glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__); \
    glfwTerminate();                                                    \
    exit(1);                                                            \
  }                                                                     \
}
