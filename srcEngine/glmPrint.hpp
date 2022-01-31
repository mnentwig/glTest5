#pragma once
#include <stdio.h>
#include <glm/fwd.hpp> // Eclipse CODAN needs this
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

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
