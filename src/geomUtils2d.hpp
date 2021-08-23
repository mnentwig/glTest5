#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "myGl.h"
#include <iostream>
class geomUtils2d {
public:

//** checks whether pt lies in triangle v1, v2, v3 in the xz plane. The y dimension is disregarded (dropping pt onto triangle) /*
  static bool pointInTriangleNoY(const glm::vec3& pt, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3){
// see https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = triCheckSignNoY (pt, v1, v2);
    d2 = triCheckSignNoY (pt, v2, v3);
    d3 = triCheckSignNoY (pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
  }

//** checks whether pt lies in triangle v1, v2, v3 in the xy plane. The z dimension is disregarded /*
  static bool pointInTriangleNoZ(const glm::vec3& pt, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3){
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = triCheckSignNoZ (pt, v1, v2);
    d2 = triCheckSignNoZ (pt, v2, v3);
    d3 = triCheckSignNoZ (pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
  }

  static void assertCoplanarityZ(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3){
    const float eps = 1e-3;
    assert(std::fabs (v1.z - v0.z) < eps);
    assert(std::fabs (v2.z - v0.z) < eps);
    assert(std::fabs (v3.z - v0.z) < eps);
  }

  static bool calcLineLineIntersectionNoZ(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, glm::vec3& outIntersection){
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
    float denomInv = 1.0f / denom;
    outIntersection.x = numX * denomInv;
    outIntersection.y = numY * denomInv;
    assert(std::fabs (denom) > 1e-6f);

#if false
    // === check range ===
    if (((outIntersection.x < v0.x) && (outIntersection.x < v1.x))
        || ((outIntersection.x < v2.x) && (outIntersection.x < v3.x))

        || ((outIntersection.y < v0.y) && (outIntersection.y < v1.y))
        || ((outIntersection.y < v2.y) && (outIntersection.y < v3.y))

        || ((outIntersection.x > v0.x) && (outIntersection.x > v1.x))
        || ((outIntersection.x > v2.x) && (outIntersection.x > v3.x))

        || ((outIntersection.y > v0.y) && (outIntersection.y > v1.y))
        || ((outIntersection.y > v2.y) && (outIntersection.y > v3.y))) {

std::cout << "intersection failed:\n";
      glmPrint(v0);
      glmPrint(v1);
      glmPrint(v2);
      glmPrint(v3);
      glmPrint(outIntersection);
      std::cout << "---\n";
      return false;
    }
#else
    float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) * denomInv;
    float u = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) * denomInv;
    if ((t < 0) || (t >= 1.0f) || (u < 0) || (t >= 1.0f)) {
      return false;
    }

#endif

    return true;
  }

protected:
  static float triCheckSignNoY(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3){
    return (p1.x - p3.x) * (p2.z - p3.z) - (p2.x - p3.x) * (p1.z - p3.z);
  }

  static float triCheckSignNoZ(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3){
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
  }
};
