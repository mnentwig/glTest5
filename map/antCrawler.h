#pragma once
#include "antCrawlerSurface.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
namespace engine {
class antCrawler {
public:
	antCrawler(antCrawlerSurface *surface, float xInit, float zInit);
	const glm::vec3& getPos();

	enum moveResult_e {
		/// movement ended in same tri as it started
		SAME_TRI,
		/// movement ended in a different tri than where it started
		OTHER_TRI,
		/// movement over the edge (clipped)
		BORDER
	};

	moveResult_e move(const glm::vec3 delta);

protected:
	int identifyVertex(surface::vertexIx_t targetIx);
	int thirdInternalIx(int internalIxA, int internalIxB);
	unsigned int locateTriByVerticalDrop(float xInit, float zInit) const;
	glm::vec2 getPosByVerticalDrop(float xInit, float zInit) const;
	void setTri(surface::triIx_t triIx);
	static bool lineLineIntersection(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3, const glm::vec2 &p4, glm::vec2 &out_tu);
	bool testCrossingAndProjectOnEdge(const glm::vec2 &v0, const glm::vec2 &v1, const glm::vec2 &startpt, const glm::vec2 &endpt, float &isBary01, float &remLengthOnv0v1, float &remLengthOnv0v1normal);

protected:
	antCrawlerSurface *surface;
	surface::triIx_t currentTriIx = 0;
	glm::mat3 m3dTo2d;
	glm::mat3 m2dTo3d;
	glm::vec2 pos_2d;
	glm::vec2 v_2d[3];
	surface::vertexIx_t v_ix[3];

	glm::vec3 posCache_3d;

	float commonZ;
	typedef uint64_t edgeHash_t;
	static edgeHash_t getEdgeHash(surface::vertexIx_t v1, surface::vertexIx_t v2);
};
}
