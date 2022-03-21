#include <src/conveyor/convMan.h>
#include <src/conveyor/convSeg.h>
#include "srcEngine/API.h"
#include <vector>
namespace game {

convMan::convMan(mgeng::root *mgengRoot, std::vector<glm::vec3> colorscheme) {
	this->straight = new mgeng::instanced(mgengRoot, /*nCols*/2, /*isOverlay*/false);

	std::vector<glm::vec3> v1;
	const float len = 12;
	const float width = 0.1;
	v1.push_back(glm::vec3(-0.25, 0.5, 0));
	v1.push_back(glm::vec3(0, 0, 0));
	v1.push_back(glm::vec3(0, 0, 12));
	v1.push_back(glm::vec3(-0.25, 0.5, 12));
	std::vector<glm::vec3> v2;
	v2.push_back(glm::vec3(0.25, 0.5, 0));
	v2.push_back(glm::vec3(0, 0, 0));
	v2.push_back(glm::vec3(0, 0, 12));
	v2.push_back(glm::vec3(0.25, 0.5, 12));

	unsigned int nV = v1.size();
	assert(v2.size() == nV);
	unsigned int colIxOutline = 0;
	unsigned int colIxFill = 1;
	bool hitscanEnable = true;
	this->straight->generateOutlinedShape(v1, width, colIxOutline, colIxFill, hitscanEnable);
	this->straight->generateOutlinedShape(v2, width, colIxOutline, colIxFill, hitscanEnable);
	//this->straight->generateOutlinedBody(v1, v2, width, colIxOutline, colIxFill, hitscanEnable);
	this->straight->finalize();

	this->colorscheme = colorscheme;
}
convMan::~convMan() {
	delete this->straight;
	for (convSeg *s : this->allConvSegs)
		delete s;
}

convSeg* convMan::newSeg_straight(glm::mat4 &model2world) {
	convSeg *s = new convSeg();
	s->model2world = model2world;
	s->inst = this->straight;
	s->setStartEndPt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 12)); // TODO
	s->setModel2world(model2world);
	this->allConvSegs.emplace(s);
	return s;
}

void convMan::connect(convSeg* sFirst, convSeg* sSecond){
	sFirst->next.push_back(sSecond);
	sSecond->prev.push_back(sFirst);
}

void convMan::render(const glm::mat4& proj){
	for (convSeg *s : this->allConvSegs){
		s->render(proj, &this->colorscheme);
	}
}
} // namespace
