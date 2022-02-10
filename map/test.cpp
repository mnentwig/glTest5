// reset ; g++ -Wall -fmax-errors=1 test.cpp
#include <vector>
#include <tuple>
#include <iostream>
#include <glm/vec3.hpp>
#include <cstdint>
#include <map>
#include <cassert>
#include <iostream> // !!
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/intersect.hpp>
#include "../srcEngine/geomUtils2d.hpp"
#include "../srcEngine/glmPrint.hpp"
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
	};

public:
	antCrawlerSurface() :
			surface() {
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
	}
};

class antCrawler {
public:
	antCrawler(antCrawlerSurface *surface, float xInit, float zInit) {
		this->surface = surface;
		const glm::vec3 *v0;
		const glm::vec3 *v1;
		const glm::vec3 *v2;
		for (unsigned int triIx = 0; triIx < this->surface->tris.size(); ++triIx) {
			this->surface->getTri(triIx, &v0, &v1, &v2);
			if (geomUtils2d::pointInTriangleNoY(glm::vec3(xInit, 0, zInit), *v0, *v1, *v2)) {
				this->currentTriIx = triIx;
				goto triLocated;
			}
		}
		assert(0 && "init point does not drop vertically onto triangle");
		triLocated: const glm::vec3 normalTri = glm::normalize(glm::cross(*v1 - *v0, *v2 - *v0));
		const glm::vec3 normalNoZ = glm::vec3(0, 0, 1);
		const glm::vec3 axis = glm::normalize(glm::cross(normalTri, normalNoZ));
		const float phi_rad = std::acos(glm::dot(normalTri, normalNoZ));
		this->m3dTo2d = glm::rotate(glm::mat4(1.0f), phi_rad, axis);
		this->m2dTo3d = glm::inverse(this->m3dTo2d);

		// === transform 3d triangle to 2d with constant z ===
		glm::vec3 v0_2dPlusZ = this->m3dTo2d * (*v0);
		this->v0_2d = v0_2dPlusZ;
		this->commonZ = v0_2dPlusZ.z; // keep z from one arbitrary vertex
		this->v1_2d = this->m3dTo2d * (*v1); // other vertices z will be identical, no need to calculate
		this->v2_2d = this->m3dTo2d * (*v2);

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
		this->pos_2d = this->m3dTo2d * pos_3d;
	}

	glm::vec3& getPos() {
		this->posCache_3d = this->m2dTo3d * glm::vec3(this->pos_2d, this->commonZ);
		return this->posCache_3d;
	}

protected:
	antCrawlerSurface *surface;
	unsigned int currentTriIx = 0;
	glm::mat3 m3dTo2d;
	glm::mat3 m2dTo3d;
	glm::vec2 pos_2d;
	glm::vec2 v0_2d;
	glm::vec2 v1_2d;
	glm::vec2 v2_2d;
	glm::vec3 posCache_3d;
	float commonZ;
};
} // namespace
int main(void) {
	engine::myAntCrawlerSurface s;
	engine::antCrawler a(&s, 0.4, 0.4);
	std::vector<std::tuple<int, int, int>> test;
	std::tuple<int, int, int> abc = { 1, 2, 3 };
	test.push_back(abc);
	return 0;
}