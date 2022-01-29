#pragma once
#include <stdio.h>
#ifndef linux
// Windows version uses glew as it comes with its own copy of relevant headers
// see also main.cpp (requires runtime initialization)
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#else
// load GLFW for windows system support but without including any openGl headers
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
// include openGLES headers ourselves
#include <GLES3/gl31.h>
#endif

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

GLint loadShaderProgFromFile(const char *vs_filename, const char *fs_filename);

#define GL_CHK(x) {x;							\
  GLenum glError = glGetError();					\
  if(glError != GL_NO_ERROR) {						\
    fprintf(stderr, "glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__); \
    glfwTerminate();							\
    exit(1);								\
  }									\
}

#define GLFW_CHK {		                                       	\
    const char* err;		                                       	\
    int code = glfwGetError(&err);                                    	\
    if (code != GLFW_NO_ERROR){						\
      fprintf(stderr, "%0x\t%s\n%s\n (%i)\n", code, err, __FILE__, __LINE__); \
      glfwTerminate();							\
      exit(1);								\
    }									\
  }
#undef GLFW_CHK
#define GLFW_CHK // not supported in older version

void glmPrint(glm::mat4 m);
void glmPrint(glm::vec3 v);
void glmPrint(glm::vec4 v);
