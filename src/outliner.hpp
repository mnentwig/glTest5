#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <iostream>
#include "instStackTriInst.h"
class outliner {
protected:
  static void calcOffsetIntersection(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, glm::vec2 n01, glm::vec2 n12, float width, glm::vec2& outIntersection){
    glm::vec2 v0a = v0 + width * n01;
    glm::vec2 v1a = v1 + width * n01;
    glm::vec2 v1b = v1 + width * n12;
    glm::vec2 v2b = v2 + width * n12;
    calcLineLineIntersection (v0a, v1a, v1b, v2b, outIntersection);
  }

  static void calcLineLineIntersection(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3, glm::vec2& outIntersection){
// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection

    float x1 = v0.x;
    float y1 = v0.y;
    float x2 = v1.x;
    float y2 = v1.y;
    float x3 = v2.x;
    float y3 = v2.y;
    float x4 = v3.x;
    float y4 = v3.y;

    float numX = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
    float numY = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);
    float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    outIntersection.x = numX / denom;
    outIntersection.y = numY / denom;
  }
  typedef struct {
    glm::vec3 vertex;
    GLuint id;
    float distToCenter;
  } fillVertex;
  public:

  static void generateFilledConvexShape(glm::vec3* vertices, int nVertices, instStackTriInst* is){
    unsigned int v0 = is->pushVertex (vertices[0]);
    unsigned int v1 = is->pushVertex (vertices[1]);

    for (int ix = 2; ix < nVertices; ++ix) {
      unsigned int v2 = is->pushVertex (vertices[ix]);
      is->pushTri (v0, v1, v2);
      v1 = v2;
    }
  }

  static void generateOutlinedShape(glm::vec3* vertices, int nVertices, float width, instStackTriInst* isOuter, instStackTriInst* isInner){
    assert(isOuter != NULL);// isInner is optional
    std::vector<glm::vec3> kneeList;

// === determine center of shape ===
    glm::vec3 center (0.0f);
    for (int ix = 0; ix < nVertices; ++ix)
      center += vertices[ix];
    center /= (float) nVertices;

    for (int ix = 0; ix < nVertices; ++ix) {
      glm::vec3 v0 = vertices[(ix - 1 + nVertices) % nVertices];
      glm::vec3 v1 = vertices[(ix + 0 + nVertices) % nVertices];
      glm::vec3 v2 = vertices[(ix + 1 + nVertices) % nVertices];

// === normal of triangle ===
      glm::vec3 triN = glm::normalize (glm::cross (v1 - v0, v2 - v1));

// === desired normal (eliminate z => 0,0,1) ===
      glm::vec3 targetN = glm::vec3 (0.0f, 0.0f, 1.0f);

// === rotation to align the two ===
// (doesn't work for parallel triN, targetN)
      glm::vec3 rotAxis = glm::cross (triN, targetN);
      float ln = length (rotAxis);
      glm::mat4 rotA;
      glm::mat4 rotB;
      if (fabs (ln) > 1e-6) {
        float phi = -acos (glm::dot (triN, targetN));
        rotAxis /= ln;
        rotA = glm::rotate (glm::mat4 (1.0f), phi, rotAxis);
        rotB = glm::rotate (glm::mat4 (1.0f), -phi, rotAxis);
      } else {
        rotA = glm::mat4 (1.0f);
        rotB = glm::mat4 (1.0f);
      }

      v0 = glm::vec3 (glm::vec4 (v0, 1.0f) * rotA);
      v1 = glm::vec3 (glm::vec4 (v1, 1.0f) * rotA);
      v2 = glm::vec3 (glm::vec4 (v2, 1.0f) * rotA);

      float commonZ = (v0.z + v1.z + v2.z) / 3.0f;// same within numerical precision
//std::cout << "Z: " << v0.z << " " << v1.z << " " << v2.z << std::endl;

// === normal vectors in plane ===
      glm::vec2 n01 = glm::normalize (glm::vec2 (-(v1 - v0).y, (v1 - v0).x));
      glm::vec2 n12 = glm::normalize (glm::vec2 (-(v2 - v1).y, (v2 - v1).x));

// === offset knee on arbitrary sides ===
      glm::vec2 i1plane;
      glm::vec2 i2plane;
      calcOffsetIntersection (v0, v1, v2, n01, n12, width, /*out*/i1plane);
      calcOffsetIntersection (v0, v1, v2, n01, n12, -width, /*out*/i2plane);

// === rotate back to 3D space ===
      glm::vec3 i1 = glm::vec3 (glm::vec4 (i1plane, commonZ, 1.0f) * rotB);
      glm::vec3 i2 = glm::vec3 (glm::vec4 (i2plane, commonZ, 1.0f) * rotB);

// === pick inner point ===
      float dsqi1 = glm::distance2 (i1, center);
      float dsqi2 = glm::distance2 (i2, center);
      glm::vec3 knee = (dsqi1 < dsqi2) ? i1 : i2;
      kneeList.push_back (knee);
    }

// === create outline polygons ===
    std::vector<GLuint> vOuter;
    std::vector<GLuint> vInner;
    for (int ix = 0; ix < nVertices; ++ix) {
      vOuter.push_back (isOuter->pushVertex (vertices[ix]));
      vInner.push_back (isOuter->pushVertex (kneeList[ix]));
    }
    for (int ix = 0; ix < nVertices; ++ix) {
      int ix2 = (ix + 1) % nVertices;
      isOuter->pushTwoTri (vOuter[ix], vOuter[ix2], vInner[ix2], vInner[ix]);
    }

    if (isInner != NULL) {
// === create fill polygons ===
      std::vector<fillVertex> vList;
      for (int ix = 0; ix < nVertices; ++ix) {
        fillVertex fv;
        fv.vertex = kneeList[ix];
        fv.id = isInner->pushVertex (fv.vertex);
        fv.distToCenter = glm::distance2 (fv.vertex, center);
        vList.push_back (fv);
      }

      while (vList.size () > 2) {
// === locate outermost vertex ===
        fillVertex vMax = vList[0];
        int ixMax = 0;
        for (unsigned int ix = 1; ix < vList.size (); ++ix)
          if (vList[ix].distToCenter > vList[ixMax].distToCenter)
            ixMax = ix;

        int ixPrev = (ixMax - 1 + vList.size ()) % vList.size ();
        int ixNext = (ixMax + 1) % vList.size ();

// === render triangle ===
        isInner->pushTri (vList[ixPrev].id, vList[ixMax].id, vList[ixNext].id);

// == clip ear ===
        vList.erase (vList.begin () + ixMax);
      }
    }
  }
#if 0
  static void generateOutlinedBody(glm::vec3* vertices1, glm::vec3* vertices2, unsigned int nVertices, float width, instStackTriInst* isOuter, instStackTriInst* isInner){
    glm::vec3 pts[4];
    for (unsigned int ix1 = 0; ix1 < nVertices; ++ix1){
      unsigned int ix2 = (ix1+1) % nVertices;
      pts[0] = vertices1[ix1];
      pts[1] = vertices1[ix2];
      pts[2] = vertices2[ix2];
      pts[3] = vertices2[ix1];
      glmPrint(pts[0]);
      glmPrint(pts[1]);
      glmPrint(pts[2]);
      glmPrint(pts[3]);
      generateOutlinedShape(pts, /*nVertices*/4, width, isOuter, isInner);
      //generateFilledConvexShape(pts, /*nVertices*/4, isOuter);
    }
  }
#endif
};
