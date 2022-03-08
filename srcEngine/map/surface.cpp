#include "surface.h"
namespace engine {
void surface::getTri(triIx_t triIx, const glm::vec3 **v0, const glm::vec3 **v1, const glm::vec3 **v2) const {
	const tri_t &t = this->tris[triIx];
	*v0 = &this->vertices[std::get<0>(t)];
	*v1 = &this->vertices[std::get<1>(t)];
	*v2 = &this->vertices[std::get<2>(t)];
}
void surface::getVertexIx(triIx_t triIx, vertexIx_t &v0, vertexIx_t &v1, vertexIx_t &v2) const {
	const tri_t &t = this->tris[triIx];
	v0 = std::get<0>(t);
	v1 = std::get<1>(t);
	v2 = std::get<2>(t);
}
}
