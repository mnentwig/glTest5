#include "myAntCrawlerSurface.h"
namespace engine {

myAntCrawlerSurface::myAntCrawlerSurface() :
		antCrawlerSurface() {
#include "map/generated.h"
	this->buildNeighborTable();
}
void myAntCrawlerSurface::plot(plotly &p) const {
	std::string id = p.getNewId();
	for (unsigned int ixTri = 0; ixTri < this->tris.size(); ++ixTri) {
		p.appendVec(id + "triVertexA", std::get<0>(this->tris[ixTri]));
		p.appendVec(id + "triVertexB", std::get<1>(this->tris[ixTri]));
		p.appendVec(id + "triVertexC", std::get<2>(this->tris[ixTri]));
		float f = (float) ixTri / this->tris.size();
		std::stringstream ss;
		ss << "'rgb(" << f << "," << f << "," << f << ")'";
		p.appendVec(id + "triColor", ss.str());
	}
	for (const glm::vec3 &v : this->vertices) {
		p.appendVec(id + "triVertexX", v.x);
		p.appendVec(id + "triVertexY", v.y);
		p.appendVec(id + "triVertexZ", v.z);
	}
	p.mesh3d(id + "triVertexX", id + "triVertexY", id + "triVertexZ", id + "triVertexA", id + "triVertexB", id + "triVertexC", id + "triColor");
}
} // namespace
