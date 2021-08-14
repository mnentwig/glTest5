#pragma once
#include "instStackLine.h"
#include "rowmans.h"
void renderText(instStackLine* is, const char* text, glm::vec3 rgb){
  float curX = 0;
  float f = 1.0/(float)rowmans_height;
  while (*text != 0){
    int glyphIx = *text - 32;
    ++text;
    assert(glyphIx >= 0);
    assert(glyphIx < rowmans_count);

    const char* data = rowmans[glyphIx];
    char nNums = rowmans_size[glyphIx];
    float glyphWidth = rowmans_width[glyphIx];
    for (int ix = 0; ix < nNums; ix += 4){
      float x1 = (float)data[ix+0];
      float y1 = -(float)data[ix+1];
      float x2 = (float)data[ix+2];
      float y2 = -(float)data[ix+3];
      
      x1 += curX;
      x2 += curX;

      const float z = 0.0f;
      int v1 = is->pushVertex(glm::vec3(f*x1, f*y1, z), rgb);
      int v2 = is->pushVertex(glm::vec3(f*x2, f*y2, z), rgb);
      is->pushLine(v1, v2);
    }
    curX += glyphWidth;
  }
}
