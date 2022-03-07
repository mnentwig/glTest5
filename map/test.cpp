// reset ; g++ -Wall -fmax-errors=1 test.cpp surface.cpp antCrawler.cpp antCrawlerSurface.cpp
#include "plotly.hpp"
#include "antCrawlerSurface.h"
#include "antCrawler.h"
namespace engine {


class myAntCrawlerSurface: public antCrawlerSurface {
public:
	myAntCrawlerSurface() :
			antCrawlerSurface() {
#include "generated.h"
		this->buildNeighborTable();
	}
	void plot(plotly &p) const {
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
};

} // namespace
int main(void) {
	try {
		plotly p("index.html");
		engine::myAntCrawlerSurface s;

		std::string id = p.getNewId();
		for (float d = 0; d < 3; d += 0.01) {
			engine::antCrawler a(&s, 0.4, 0.4);
			a.move(glm::vec3(d, 0, 0));
			glm::vec3 pos = a.getPos();
			p.appendVecXYZ(id + "path", pos);
		}
		p.scatter3d(id + "path");

		s.plot(p);
		p.close();

		std::cout << "done\n";
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
