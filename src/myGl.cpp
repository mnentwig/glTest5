#include "myGl.h"
#include <fstream>
#include <string>
static std::string get_file_string(const char* filename){
    std::ifstream ifs(filename);
    return std::string((std::istreambuf_iterator<char>(ifs)),
                  (std::istreambuf_iterator<char>()));
}

GLint loadShaderProgFromFile(const char *vs_filename, const char *fs_filename) {
  std::string vs = get_file_string(vs_filename);
  std::string fs = get_file_string(fs_filename);

  enum Consts {INFOLOG_LEN = 512};
    GLchar infoLog[INFOLOG_LEN];
    GLint fragment_shader;
    GLint shader_program;
    GLint success;
    GLint vertex_shader;
    const char* vshader = vs.c_str();
    const char* fshader = fs.c_str();
    
    /* Vertex shader */
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vshader, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
        printf("error (%s): vertex shader compilation failed:\n%s\n", vs_filename, infoLog);
	exit(EXIT_FAILURE);
    }

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fshader, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
        printf("error (%s): fragment shader compilation failed\n%s\n", fs_filename, infoLog);
	exit(EXIT_FAILURE);
    }

    /* Link shaders */
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
        printf("error (%s/%s): linking failed\n%s\n", vs_filename, fs_filename, infoLog);
	exit(EXIT_FAILURE);
    }

    glDeleteShader(vertex_shader); // flags for deletion on glDeleteProgram
    glDeleteShader(fragment_shader);
    return shader_program;
}

void glmPrint(glm::mat4 m){
  printf("%01.3f\t%01.3f\t%01.3f\t%01.3f\n",m[0].x, m[1].x, m[2].x, m[3].x);
  printf("%01.3f\t%01.3f\t%01.3f\t%01.3f\n",m[0].y, m[1].y, m[2].y, m[3].y);
  printf("%01.3f\t%01.3f\t%01.3f\t%01.3f\n",m[0].z, m[1].z, m[2].z, m[3].z);
  printf("%01.3f\t%01.3f\t%01.3f\t%01.3f\n",m[0].w, m[1].w, m[2].w, m[3].w);
}

void glmPrint(glm::vec3 v){
  printf("%01.6f\t%01.6f\t%01.6f\n",v.x, v.y, v.z);
}

void glmPrint(glm::vec4 v){
  printf("%01.3f\t%01.3f\t%01.3f\t%01.3f\n",v.x, v.y, v.z, v.w);
}

