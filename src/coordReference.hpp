#pragma once
#include "instStackLine.h"
#include "renderText.hpp"
class coordReference{
protected:
  instStackLine isO;
  instStackLine isX;
  instStackLine isY;
  instStackLine isZ;
  instStackLine isAxes;
  static glm::vec3 getScreenCoord(glm::vec3 point, glm::mat4 proj){
    glm::vec4 tmp = proj*glm::vec4(point, 1.0f);
    tmp *= 1.0f/tmp.w;
    return glm::vec3(tmp.x, tmp.y, tmp.z);
  }
public:
  coordReference(){
    glm::vec3 rgb(1, 1, 1);
    renderText(&this->isO, "O", rgb);
    renderText(&this->isX, "X", rgb);
    renderText(&this->isY, "Y", rgb);
    renderText(&this->isZ, "Z", rgb);

    int v0 = this->isAxes.pushVertex(glm::vec3(0, 0, 0), rgb);
    int vx = this->isAxes.pushVertex(glm::vec3(1, 0, 0), rgb);
    int vy = this->isAxes.pushVertex(glm::vec3(0, 1, 0), rgb);
    int vz = this->isAxes.pushVertex(glm::vec3(0, 0, 1), rgb);

    this->isAxes.pushLine(v0, vx);
    this->isAxes.pushLine(v0, vy);
    this->isAxes.pushLine(v0, vz);

    this->isO.finalize();
    this->isX.finalize();
    this->isY.finalize();
    this->isZ.finalize();
    this->isAxes.finalize();
  }

  void run(glm::mat4 proj, float viewportX, float viewportY){
    
    this->isAxes.run(proj, viewportX, viewportY);
    
    float textTagScale = 0.07;
    glm::mat4 projTextTag = glm::mat4(1.0f);
    projTextTag[0].x = textTagScale;
    projTextTag[1].y = textTagScale;
    projTextTag[2].z = textTagScale;

    glm::vec3 oScreen = getScreenCoord(glm::vec3(0, 0, 0), proj);
    if (oScreen.z < 1){
      projTextTag[3].x = oScreen.x;
      projTextTag[3].y = oScreen.y;
      projTextTag[3].z = oScreen.z;
      this->isO.run(projTextTag, viewportX, viewportY);
    }
    
    glm::vec3 xScreen = getScreenCoord(glm::vec3(1, 0, 0), proj);
    if (xScreen.z < 1){
      projTextTag[3].x = xScreen.x;
      projTextTag[3].y = xScreen.y;
      projTextTag[3].z = xScreen.z;
      this->isX.run(projTextTag, viewportX, viewportY);
    }

    glm::vec3 yScreen = getScreenCoord(glm::vec3(0, 1, 0), proj);
    if (yScreen.z < 1){
      projTextTag[3].x = yScreen.x;
      projTextTag[3].y = yScreen.y;
      projTextTag[3].z = yScreen.z;
      this->isY.run(projTextTag, viewportX, viewportY);
    }
    
    glm::vec3 zScreen = getScreenCoord(glm::vec3(0, 0, 1), proj);
    if (zScreen.z < 1){
      projTextTag[3].x = zScreen.x;
      projTextTag[3].y = zScreen.y;
      projTextTag[3].z = zScreen.z;
      this->isZ.run(projTextTag, viewportX, viewportY);    
    }
  }
};
