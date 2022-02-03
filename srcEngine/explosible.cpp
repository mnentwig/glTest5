#include "API.h"
#include "engine.h"
#include "explosible.h"
#include "instStackTriInst.h"
#include "outliner.hpp"
#include "instMan.h"
#include "explTraj.h"
#include <math.h>
#include <vector>
#include <assert.h>
#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>

namespace engine {
blueprint::blueprint(instMan *im, unsigned int nCol) {
	this->im = im;
	this->imHandle = im->openHandle(nCol, false);
}

void blueprint::finalize() {
	unsigned int nCol = this->im->getNCol(this->imHandle);
	for (unsigned int ixCol = 0; ixCol < nCol; ++ixCol) {
		this->im->getIsti(this->imHandle, ixCol)->finalize();
	}
}

void blueprint::render(const glm::mat4 &proj, const std::vector<glm::vec3> &rgb) {
	this->im->renderInst(this->imHandle, proj, rgb);
}

blueprintHitscan::blueprintHitscan(instMan *im, unsigned int nCol) :
		blueprint(im, nCol) {
}

void blueprintHitscan::addHitscanSurface(const std::vector<glm::vec3> vertices) {
	this->hitscanSurfaces.push_back(vertices);

	// === calculate center-of-gravity ===
	glm::vec3 cog(0, 0, 0);
	for (unsigned int ix = 0; ix < vertices.size(); ++ix) {
		cog += vertices[ix];
	}
	cog /= (float) vertices.size();

	// === create collision detect triangles ===
	typedef struct {
		glm::vec3 vertex;
		float distToCenter;
	} earClipVertex;

	std::vector<earClipVertex> vList;
	for (unsigned int ix = 0; ix < vertices.size(); ++ix) {
		earClipVertex v;
		v.vertex = vertices[ix];
		v.distToCenter = glm::distance2(v.vertex, cog);
		vList.push_back(v);
	}

	// === clip ears ===
	while (vList.size() > 2) {
		// === locate outermost vertex ===
		int ixMax = 0;
		for (unsigned int ix = 1; ix < vList.size(); ++ix)
			if (vList[ix].distToCenter > vList[ixMax].distToCenter)
				ixMax = ix;

		// === get triangle where ixMax identifies the outermost point ===
		int ixPrev = (ixMax - 1 + vList.size()) % vList.size();
		int ixNext = (ixMax + 1) % vList.size();

		// === add to collision detect triangles ===
		collisionTri t;
		t.p0 = vList[ixPrev].vertex;
		t.p1 = vList[ixMax].vertex;
		t.p2 = vList[ixNext].vertex;
		this->collisionTriList.push_back(t);

		// == clip ear ===
		vList.erase(vList.begin() + ixMax);
	}
}

static glm::vec3 matMul(const glm::mat4 &m, const glm::vec3 &v) {
	return glm::vec3(m * glm::vec4(v, 1.0f));
}

float blueprintHitscan::lineIntersectAtDistance(const glm::mat4 &proj, const glm::vec3 &lineOrig, const glm::vec3 &lineDir) {
	float distMax = glm::length(lineDir);
	float dBest = distMax + 1.234f;
	for (unsigned int ix = 0; ix < this->collisionTriList.size(); ++ix) {
		collisionTri t = this->collisionTriList[ix];
		glm::vec2 isBary;
		GLfloat d;
		if (glm::intersectRayTriangle(lineOrig, lineDir, matMul(proj, t.p0), matMul(proj, t.p1), matMul(proj, t.p2), isBary, d)) {
			if ((d >= 0) && (d < dBest))
				dBest = d;
		}
	}
	return dBest <= distMax ? dBest : NAN;
}

/** One surface of an "explosible" object that may fly away, spinning wildly
 *  Dual purpose: Hitscan detection
 */
class fragment {
public:
	fragment(const std::vector<unsigned int> startIx) {
		this->startIx = startIx;
		this->vertices = new std::vector<glm::vec3>();
	}

	void addVertex(glm::vec3 &v) {
		assert(!this->closed);
		this->vertices->push_back(v);
	}

	void close(std::vector<unsigned int> endIx) {
		assert(!this->closed);
		this->closed = true;
		this->endIx = endIx;

// === calculate center-of-gravity ===
		this->cog = glm::vec3(0, 0, 0);
		for (unsigned int ix = 0; ix < this->vertices->size(); ++ix) {
			this->cog += (*this->vertices)[ix];
		}
		this->cog /= (float) this->vertices->size();
	}

	/** returns (approximate?) center-of-gravity */
	glm::vec3 getCog() const {
		return this->cog;
	}
	/** returns translation matrix to move centered object back to its original location */
	glm::mat4 getCenterCog() const {
		return glm::translate(glm::mat4(1.0f), -this->cog);
	}
	/** returns translation matrix to move object's COG to coordinate origin for rotation */
	glm::mat4 getUncenterCog() const {
		return glm::translate(glm::mat4(1.0f), this->cog);
	}

	/** returns a rotation axis for the part spinning while flying away */
	glm::vec3 getAxis() const {
		glm::vec3 r = (*this->vertices)[0] - this->cog;
		return glm::normalize(r);
	}

	std::vector<unsigned int> startIx;
	std::vector<unsigned int> endIx;
protected:
	std::vector<glm::vec3> *vertices;
	glm::vec3 cog;
	bool closed = false;
};

explosible::explosible(instMan *im, unsigned int nCol) :
		blueprintHitscan(im, nCol) {
	std::vector<unsigned int> startIx;
	for (unsigned int ixCol = 0; ixCol < nCol; ++ixCol) {
		startIx.push_back(this->im->getIsti(this->imHandle, ixCol)->getTriCount());
	}
	this->currentFragment = new fragment(startIx);
}

void explosible::generateOutlinedShape(std::vector<glm::vec3> vertices, float width, unsigned int ixColOutline, unsigned int ixColFill, bool hitscanEnable) {
	instStackTriInst *isOutline = this->im->getIsti(this->imHandle, ixColOutline);
	instStackTriInst *isFill = this->im->getIsti(this->imHandle, ixColFill);
	if (hitscanEnable) {
		this->addHitscanSurface(vertices);
	}
	std::vector<glm::vec3> vertOutline;
	std::vector<mgeng::triIx16> triOutline;
	std::vector<glm::vec3> vertFill;
	std::vector<mgeng::triIx16> triFill;
	outliner::generateOutlinedShape(vertices, width, vertOutline, triOutline, vertFill, triFill);
	isOutline->pushTris(vertOutline, triOutline, /*rebaseZeroBasedIndices*/true);
	isFill->pushTris(vertFill, triFill, /*rebaseZeroBasedIndices*/true);
	for (unsigned int ix = 0; ix < vertices.size(); ++ix)
		this->currentFragment->addVertex(vertices[ix]);
	this->closeFragment();
}

void explosible::generateOutlinedBody(std::vector<glm::vec3> vertices1, std::vector<glm::vec3> vertices2, float width, unsigned int ixColOutline, unsigned int ixColFill, bool hitscanEnable) {
	instStackTriInst *isOutline = this->im->getIsti(this->imHandle, ixColOutline);
	instStackTriInst *isFill = this->im->getIsti(this->imHandle, ixColFill);
	std::vector<glm::vec3> pts(4);
	unsigned int nVertices = vertices1.size();
	for (unsigned int ix1 = 0; ix1 < nVertices; ++ix1) {
		unsigned int ix2 = (ix1 + 1) % nVertices;
		pts[0] = vertices1[ix1];
		pts[1] = vertices1[ix2];
		pts[2] = vertices2[ix2];
		pts[3] = vertices2[ix1];
		if (hitscanEnable) {
			this->addHitscanSurface(pts);
		}
		std::vector<glm::vec3> vertOutline;
		std::vector<mgeng::triIx16> triOutline;
		std::vector<glm::vec3> vertFill;
		std::vector<mgeng::triIx16> triFill;
		outliner::generateOutlinedShape(pts, width, vertOutline, triOutline, vertFill, triFill);
		isOutline->pushTris(vertOutline, triOutline, /*rebaseZeroBasedIndices*/true);
		isFill->pushTris(vertFill, triFill, /*rebaseZeroBasedIndices*/true);

		this->currentFragment->addVertex(pts[0]);
		this->currentFragment->addVertex(pts[1]);
		this->currentFragment->addVertex(pts[2]);
		this->currentFragment->addVertex(pts[3]);
		this->closeFragment();
	}
} // namespace

void explosible::closeFragment() {
	std::vector<unsigned int> endIx;
	unsigned int nCol = this->im->getNCol(this->imHandle);
	for (unsigned int ixCol = 0; ixCol < nCol; ++ixCol) {
		endIx.push_back(this->im->getIsti(this->imHandle, ixCol)->getTriCount());
	}
	this->currentFragment->close(endIx);
	this->fragments.push_back(this->currentFragment);
	this->currentFragment = new fragment(/*becomes new frag startIx*/endIx); // TODO maybe open-/close fragment explicitly?
}

void explosible::renderExplosion(const glm::mat4 &model2screen, const glm::mat4 &model2model, const explTraj *traj, const std::vector<glm::vec3> &rgb) {
	unsigned int nCol = this->im->getNCol(this->imHandle);
	for (unsigned int ix = 0; ix < this->fragments.size(); ++ix) {
		fragment *f = this->fragments[ix];
		glm::mat4 model2model_expl = glm::translate(glm::mat4(1.0f), traj->getDeltaPos(ix));
		glm::mat4 tot = model2screen;
		tot *= model2model_expl;
		tot *= model2model;
		tot *= f->getUncenterCog();
		tot *= traj->getRotation(ix);
		tot *= f->getCenterCog();
		for (unsigned int ixCol = 0; ixCol < nCol; ++ixCol) {
			this->im->getIsti(this->imHandle, ixCol)->run1(f->startIx[ixCol], f->endIx[ixCol], tot, rgb[ixCol]);
		}
	}
}

void explosible::explode(explTraj *traj, glm::vec3 impact, float speed, float angSpeed) {
	traj->clear();
	for (unsigned int ix = 0; ix < this->fragments.size(); ++ix) {
		glm::vec3 dir = this->fragments[ix]->getCog() - impact;
		dir = glm::normalize(dir);
		glm::vec3 axis = this->fragments[ix]->getAxis();
		traj->registerFragment(dir, speed, axis, angSpeed);
	}
}
} //namespace

mgeng::instanced::instanced(mgeng::root *root, unsigned int nCol) {
	this->ex = new engine::explosible(&root->eng->im, nCol);
}
//TODO add const below to params
void mgeng::instanced::generateOutlinedShape(std::vector<glm::vec3> vertices, float width, unsigned int ixColOutline, unsigned int ixColFill, bool hitscanEnable) {
	this->ex->generateOutlinedShape(vertices, width, ixColOutline, ixColFill, hitscanEnable);
}
void mgeng::instanced::generateOutlinedBody(std::vector<glm::vec3> vertices1, std::vector<glm::vec3> vertices2, float width, unsigned int ixColOutline, unsigned int ixColFill, bool hitscanEnable) {
	this->ex->generateOutlinedBody(vertices1, vertices2, width, ixColOutline, ixColFill, hitscanEnable);
}
void mgeng::instanced::finalize() {
	this->ex->finalize();
}
void mgeng::instanced::render(glm::mat4 proj, std::vector<glm::vec3> col) {
	this->ex->render(proj, col);
}
void mgeng::instanced::renderExplosion(const instancedExplosion* traj, const glm::mat4 &model2screen, const glm::mat4 &model2model, const std::vector<glm::vec3> &rgb){
	this->ex->renderExplosion(model2screen, model2model, traj->traj, rgb);
}
void mgeng::instanced::explode(mgeng::instancedExplosion *traj, glm::vec3 impact, float speed, float angSpeed){
	this->ex->explode(traj->traj, impact, speed, angSpeed);
}
