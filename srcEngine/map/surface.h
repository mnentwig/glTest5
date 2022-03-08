#pragma once
#include <vector>
#include <tuple>
#include <glm/vec3.hpp>

namespace engine {
class surface {
public:
	typedef unsigned int vertexIx_t;
	typedef unsigned int triIx_t;
	typedef std::tuple<vertexIx_t, vertexIx_t, vertexIx_t> tri_t;
	std::vector<glm::vec3> vertices;
	std::vector<tri_t> tris;
	void getTri(triIx_t triIx, const glm::vec3 **v0, const glm::vec3 **v1, const glm::vec3 **v2) const;
	void getVertexIx(triIx_t triIx, vertexIx_t &v0, vertexIx_t &v1, vertexIx_t &v2) const;
};
}
