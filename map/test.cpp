// reset ; g++ -Wall -fmax-errors=1 test.cpp
#include <vector>
#include <tuple>
#include <iostream>
#include <glm/vec3.hpp>
#include <cstdint>
#include <map>
#include <cassert>
#include <iostream> // !!
//#include <sstream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/intersect.hpp>
#include "../srcEngine/geomUtils2d.hpp"
#include "../srcEngine/glmPrint.hpp"
#include "plotly.hpp"
namespace engine {
class surface {
public:
	typedef unsigned int vertexIx_t;
	typedef unsigned int triIx_t;
	typedef std::tuple<vertexIx_t, vertexIx_t, vertexIx_t> tri_t;
	std::vector<glm::vec3> vertices;
	std::vector<tri_t> tris;
	void getTri(triIx_t triIx, const glm::vec3 **v0, const glm::vec3 **v1, const glm::vec3 **v2) const {
		const tri_t &t = this->tris[triIx];
		*v0 = &this->vertices[std::get<0>(t)];
		*v1 = &this->vertices[std::get<1>(t)];
		*v2 = &this->vertices[std::get<2>(t)];
	}
	void getVertexIx(triIx_t triIx, vertexIx_t &v0, vertexIx_t &v1, vertexIx_t &v2) {
		const tri_t &t = this->tris[triIx];
		v0 = std::get<0>(t);
		v1 = std::get<1>(t);
		v2 = std::get<2>(t);
	}
};

class antCrawlerSurface: public surface {
	class neighborTable_cl {
		typedef std::tuple<triIx_t, triIx_t> edge_t;
		typedef uint64_t edgeHash_t;
	public:
		void registerTri(triIx_t triIx, tri_t &tri) {
			edgeHash_t eh01 = neighborTable_cl::getEdgeHash(std::get<0>(tri), std::get<1>(tri));
			edgeHash_t eh12 = neighborTable_cl::getEdgeHash(std::get<1>(tri), std::get<2>(tri));
			edgeHash_t eh20 = neighborTable_cl::getEdgeHash(std::get<2>(tri), std::get<0>(tri));
			this->registerEdge(eh01, triIx);
			this->registerEdge(eh12, triIx);
			this->registerEdge(eh20, triIx);
		}
		bool getNeighbor(vertexIx_t va, vertexIx_t vb, triIx_t startTri, triIx_t &neighborTri) const {
			edgeHash_t eh = getEdgeHash(va, vb);
			auto it = this->table.find(eh);
			if (it == this->table.end())
				return false;
			const edge_t &e = (*it).second;
			triIx_t i1 = std::get<0>(e);
			if (i1 != startTri) {
				neighborTri = i1;
				return true;
			}
			triIx_t i2 = std::get<1>(e);
			if (i2 != startTri) {
				neighborTri = i2;
				return true;
			}
			return false;
		}
	protected:
		std::map<edgeHash_t, edge_t> table;
		void registerEdge(edgeHash_t eh, triIx_t triIx) {
			auto it = this->table.find(eh);
			if (it == this->table.end()) {
				this->table[eh] = { triIx, triIx }; // one valid neighbor => set the same twice
			} else {
				edge_t &e = (*it).second;
				assert(std::get<0>(e) == std::get<1>(e)); // check for one valid neighbor
				std::get<1>(e) = triIx;
			}
		}

		static edgeHash_t getEdgeHash(vertexIx_t v1, vertexIx_t v2) {
			assert(v1 != v2);
			if (v1 < v2)
				return (edgeHash_t) v1 | (edgeHash_t) v2 << 32;
			else
				return (edgeHash_t) v2 | (edgeHash_t) v1 << 32;
		}
		static bool hasTwoNeighbors(edgeHash_t h) {
			return (h >> 32) != (h & 0xFFFFFFFF);
		}
	}; // class neighborTable

public:
	antCrawlerSurface() :
			surface() {
	}
	bool getNeighbor(vertexIx_t va, vertexIx_t vb, triIx_t startTri, triIx_t &neighborTri) const {
		return this->neighborTable.getNeighbor(va, vb, startTri, neighborTri);
	}

protected:
	neighborTable_cl neighborTable;
	void buildNeighborTable() {
		for (unsigned int ixTri = 0; ixTri < this->tris.size(); ++ixTri) {
			this->neighborTable.registerTri(ixTri, this->tris[ixTri]);
		}
	}
};

class myAntCrawlerSurface: public antCrawlerSurface {
public:
	myAntCrawlerSurface() :
			antCrawlerSurface() {
#include "generated.h"
		this->buildNeighborTable();
		plotly p("index.html");
		for (unsigned int ixTri = 0; ixTri < this->tris.size(); ++ixTri) {
			p.appendVec("triVertexA", std::get<0>(this->tris[ixTri]));
			p.appendVec("triVertexB", std::get<1>(this->tris[ixTri]));
			p.appendVec("triVertexC", std::get<2>(this->tris[ixTri]));
			float f = (float) ixTri / this->tris.size();
			std::stringstream ss;
			ss << "'rgb(" << f << "," << f << "," << f << ")'";
			p.appendVec("triColor", ss.str());
		}
		for (glm::vec3 &v : this->vertices) {
			p.appendVec("triVertexX", v.x);
			p.appendVec("triVertexY", v.y);
			p.appendVec("triVertexZ", v.z);
		}
		p.close();
	}
};

class antCrawler {
public:
	antCrawler(antCrawlerSurface *surface, float xInit, float zInit) {
		this->surface = surface;

		unsigned int droppedTriIx = this->locateTriByVerticalDrop(xInit, zInit);
		this->setTri(droppedTriIx);
		this->pos_2d = this->getPosByVerticalDrop(xInit, zInit);
	}

	const glm::vec3& getPos() {
		this->posCache_3d = this->m2dTo3d * glm::vec3(this->pos_2d, this->commonZ);
		return this->posCache_3d;
	}

	enum moveResult_e {
		/// movement ended in same tri as it started
		SAME_TRI,
		/// movement ended in a different tri than where it started
		OTHER_TRI,
		/// movement over the edge (clipped)
		OFF_SURFACE
	};

	moveResult_e move(const glm::vec3 delta) { // return quat?
		glm::vec2 dir_2d = this->m3dTo2d * delta;
		moveResult_e res = moveResult_e::SAME_TRI;
		while (true) {
			glm::vec2 newPos_2d = this->pos_2d + dir_2d;

			// === check whether the movement crosses an edge (leaves the triangle) ===
			bool edgeCrossed = false;
			float remLengthParallelToEdge;
			float remLengthOrthogonalToEdge;
			surface::vertexIx_t va_ix;
			surface::vertexIx_t vb_ix;
			float isBaryAB;

			for (surface::vertexIx_t ixVa = 0; ixVa < 3; ++ixVa) {
				surface::vertexIx_t ixVb = ixVa < 2 ? ixVa + 1 : 0;
				if (moveOverEdge(this->v_2d[ixVa], this->v_2d[ixVb], this->pos_2d, newPos_2d, /*out*/isBaryAB, remLengthParallelToEdge, remLengthOrthogonalToEdge)) {
					edgeCrossed = true;
					va_ix = this->v_ix[ixVa];
					vb_ix = this->v_ix[ixVb];
					std::cout << ixVa << ixVb << "\n";
					break;
				}
			}
			if (!edgeCrossed) {
				this->pos_2d = newPos_2d;
				return res;
			}

			surface::triIx_t neighborTri;
			if (!this->surface->getNeighbor(va_ix, vb_ix, this->currentTriIx, neighborTri)) {
				return moveResult_e::OFF_SURFACE;
			}

			this->setTri(neighborTri);
			res = moveResult_e::OTHER_TRI;

			// === identify the vertices of the new "current" tri ===
			int internalIxA = this->identifyVertex(va_ix); // first point on neighbor edge
			int internalIxB = this->identifyVertex(vb_ix); // second point on neighbor edge
			int internalIxC = this->identifyVertex(internalIxA, internalIxB); // non-adjacent point

			// === calculate unit vectors of the new tri corresponding to remLengthParallelToEdge / remLengthOrthogonalToEdge
			glm::vec2 unitDirParallelToEdge;
			glm::vec2 unitDirOrthogonalToEdge;
			geomUtils2d::orthogonalize(this->v_2d[internalIxA], this->v_2d[internalIxB], this->v_2d[internalIxC], /*out*/unitDirParallelToEdge, unitDirOrthogonalToEdge);

			// === set new position on common edge ===
			this->pos_2d = (1.0f - isBaryAB) * this->v_2d[internalIxA] + isBaryAB * this->v_2d[internalIxB];

			// === update direction ===
			dir_2d = remLengthParallelToEdge * unitDirParallelToEdge + remLengthOrthogonalToEdge * unitDirOrthogonalToEdge;
		}
	}

protected:
	int identifyVertex(surface::vertexIx_t targetIx) {
		for (int internalIx = 0; internalIx < 3; ++internalIx)
			if (this->v_ix[internalIx] == targetIx)
				return internalIx;
		throw std::runtime_error("identify vertex: not found");
	}

	int identifyVertex(int internalIxA, int internalIxB) {
		if ((internalIxA == 0) && (internalIxB == 1))
			return 2;
		if ((internalIxA == 0) && (internalIxB == 2))
			return 1;
		if ((internalIxA == 1) && (internalIxB == 0))
			return 2;
		if ((internalIxA == 1) && (internalIxB == 2))
			return 0;
		if ((internalIxA == 2) && (internalIxB == 0))
			return 1;
		if ((internalIxA == 2) && (internalIxB == 1))
			return 0;
		throw std::runtime_error("identify vertex: invalid args");
	}
	unsigned int locateTriByVerticalDrop(float xInit, float zInit) const {
		const glm::vec3 *v0;
		const glm::vec3 *v1;
		const glm::vec3 *v2;
		for (unsigned int triIx = 0; triIx < this->surface->tris.size(); ++triIx) {
			this->surface->getTri(triIx, &v0, &v1, &v2);
			if (geomUtils2d::pointInTriangleNoY(glm::vec3(xInit, 0, zInit), *v0, *v1, *v2)) {
				return triIx;
			}
		}
		throw std::runtime_error("failed to locate tri by vertical drop");
	}

	glm::vec2 getPosByVerticalDrop(float xInit, float zInit) const {
		const glm::vec3 *v0;
		const glm::vec3 *v1;
		const glm::vec3 *v2;
		this->surface->getTri(this->currentTriIx, &v0, &v1, &v2);

		// Need all three coordinates of the initial point to transform to 2d
		// Note: The "ray" in intersectRayTriangle is bidirectional.
		// With the startpoint at y=0, "dist" gives  the y-coordinate.
		glm::vec3 rayOrig = glm::vec3(xInit, 0, zInit);
		glm::vec3 rayDir = glm::vec3(0, 1, 0);
		glm::vec2 baryPosition;
		float dist;
		bool hit = glm::intersectRayTriangle(rayOrig, rayDir, *v0, *v1, *v2, baryPosition, dist);
		std::cout << hit << " " << dist << "\n";
		glm::vec3 pos_3d(xInit, dist, zInit);
		return this->m3dTo2d * pos_3d;
	}

	void setTri(surface::triIx_t triIx) {
		this->currentTriIx = triIx;

		const glm::vec3 *v0;
		const glm::vec3 *v1;
		const glm::vec3 *v2;
		this->surface->getTri(triIx, &v0, &v1, &v2);

		this->surface->getVertexIx(triIx, /*out*/this->v_ix[0], this->v_ix[1], this->v_ix[2]);
		// === normal vector in 3d ===
		const glm::vec3 normalTri = glm::normalize(glm::cross(*v1 - *v0, *v2 - *v0));
		// === targeted normal vector in 2d when z is eliminated ===
		const glm::vec3 normalNoZ = glm::vec3(0, 0, 1);
		// === rotation axis to rotate normalTri into normalNoZ ===
		const glm::vec3 axis = glm::normalize(glm::cross(normalTri, normalNoZ));
		// === rotation angle to rotate normalTri into normalNoZ ===
		const float phi_rad = std::acos(glm::dot(normalTri, normalNoZ));
		// === rotation matrix to rotate normalTri into normalNoZ ===
		this->m3dTo2d = glm::rotate(glm::mat4(1.0f), phi_rad, axis);
		// === rotation matrix to rotate normalNoZ into normalTri ===
		this->m2dTo3d = glm::inverse(this->m3dTo2d);

		// === transform 3d triangle to 2d with constant z ===
		glm::vec3 v0_2dPlusZ = this->m3dTo2d * (*v0);
		this->commonZ = v0_2dPlusZ.z; // keep z from one arbitrary vertex
		this->v_2d[0] = v0_2dPlusZ;
		this->v_2d[1] = this->m3dTo2d * (*v1); // other vertices z will be identical, no need to calculate
		this->v_2d[2] = this->m3dTo2d * (*v2);
	}

	static bool lineLineIntersection(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3, const glm::vec2 &p4, glm::vec2 &out_tu) {
		const float dx13 = (p1.x - p3.x);
		const float dy13 = (p1.y - p3.y);
		const float dy34 = (p3.y - p4.y);
		const float dx34 = (p3.x - p4.x);
		const float dx12 = (p1.x - p2.x);
		const float dy12 = (p1.y - p2.y);
		float denom = dx12 * dy34 - dy12 * dx34;
		float num_t = dx13 * dy34 - dy13 * dx34;
		float num_u = dx13 * dy12 - dy13 * dx12;
		if (fabs(denom) <= std::numeric_limits<float>::epsilon())
			return false;
		float t = num_t / denom;
		if ((t < 0) || (t > 1))
			return false;
		float u = num_u / denom;
		if ((u < 0) || (u > 1))
			return false;
		out_tu = glm::vec2(t, u);
		return true;
	}

	/// checks whether the line B=[startpt, endpt] crosses the line A=[v0, v1],
	/// Returns true, if intersection. In this case, the remaining length of B is returned projected on A and its normal.
	bool moveOverEdge(const glm::vec2 &v0, const glm::vec2 &v1, const glm::vec2 &startpt, const glm::vec2 &endpt, float &isBary01, float &remLengthOnv0v1, float &remLengthOnv0v1normal) {
		glm::vec2 out_tu;
		if (!lineLineIntersection(v0, v1, startpt, endpt, out_tu))
			return false;

		// === calculate intersection point from returned barycentric coordinates ===
		isBary01 = out_tu[0];
		glm::vec2 isPt = (1.0f - isBary01) * v0 + isBary01 * v1;
		glm::vec2 rem = endpt - isPt;

		// === project rem on [v0, v1] ===
		glm::vec2 v0v1norm = glm::normalize(v1 - v0);
		glm::vec2 remParallel = v0v1norm * glm::dot(v0v1norm, rem);
		glm::vec2 remOrthogonal = rem - remParallel;
		remLengthOnv0v1 = glm::length(remParallel);
		remLengthOnv0v1normal = glm::length(remOrthogonal);
		return true;
	}

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
};
} // namespace
int main(void) {
	try {
		engine::myAntCrawlerSurface s;
		engine::antCrawler a(&s, 0.4, 0.4);
		std::cout << a.move(glm::vec3(10, 0, 0)) << std::endl;

		return EXIT_SUCCESS;
#if 0
	catch (const std::runtime_error &e) {
		std::cout << "runtime error:" << e.what() << std::endl;
		return EXIT_FAILURE;
	}
#else
	} catch (char a) {
		// dummy
	}
#endif
}
