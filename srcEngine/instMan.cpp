#define GLM_FORCE_RADIANS
#include "instMan.h"
#include <glm/glm.hpp>
#include "instStackTriInst.h"
#include <vector>
#include <iostream>
class instTemplate {
public:
	std::vector<glm::mat4> proj;
	std::vector<std::vector<glm::vec3>> cols;
	std::vector<instStackTriInst*> isti;
	bool overlayMode;
};

instMan::instMan() {
	this->nHandles = 0;
	this->frameIsOn = false;
}

unsigned int instMan::openHandle(unsigned int nCol, bool isOverlay) {
	std::cout << this->nHandles << std::endl;
	instTemplate *t = new instTemplate();
	t->overlayMode = isOverlay;
	for (unsigned int ixCol = 0; ixCol < nCol; ++ixCol) {
		t->isti.push_back(new instStackTriInst());
		t->cols.push_back(std::vector<glm::vec3>());
	}
	this->templates.push_back(t);
	return this->nHandles++;
}

instStackTriInst* instMan::getIsti(unsigned int handle, int ixCol) const {
	return this->templates[handle]->isti[ixCol];
}

void instMan::startFrame() {
	assert(!this->frameIsOn);
	this->frameIsOn = true;

	for (unsigned int ix = 0; ix < this->nHandles; ++ix) {
		instTemplate *t = this->templates[ix];
		t->proj.clear();
		for (unsigned int ixCol = 0; ixCol < t->cols.size(); ++ixCol) {
			t->cols[ixCol].clear();
		}
	}
}

void instMan::renderInst(unsigned int handle, const glm::mat4 &proj, const std::vector<glm::vec3> rgb) {
	assert(this->frameIsOn);
	instTemplate *t = this->templates[handle];
	assert(rgb.size() == t->isti.size());

	t->proj.push_back(proj);
	for (unsigned int ixIs = 0; ixIs < t->isti.size(); ++ixIs) {
		t->cols[ixIs].push_back(rgb[ixIs]);
	}

	this->templates[handle]->cols.push_back(rgb);
}

void instMan::endFrame() {
	assert(this->frameIsOn);
	this->frameIsOn = false;

	for (auto it = this->templates.begin(); it != this->templates.end(); ++it) {
		instTemplate *t = *it;
		glm::mat4 *proj = t->proj.data();
		int nInst = t->proj.size();
		for (unsigned int ixIs = 0; ixIs < t->isti.size(); ++ixIs) {
			if (t->overlayMode) {
				t->isti[ixIs]->runOverlay(proj, t->cols[ixIs].data(), nInst);
			} else {
				t->isti[ixIs]->run(proj, t->cols[ixIs].data(), nInst);
			}
		}
	}
}

void instMan::shutdown() {
	for (auto it = this->templates.begin(); it != this->templates.end(); ++it) {
		instTemplate *t = *it;
		for (unsigned int ixIs = 0; ixIs < t->isti.size(); ++ixIs) {
			t->isti[ixIs]->shutdown();
		}
	}
}

instMan::~instMan() {
	for (auto it = this->templates.begin(); it != this->templates.end(); ++it) {
		instTemplate *t = *it;
		for (unsigned int ixIs = 0; ixIs < t->isti.size(); ++ixIs) {
			delete t->isti[ixIs];
		}
	}
}
