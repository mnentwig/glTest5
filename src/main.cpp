#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include <GLFW/glfw3.h> // for keycodes
#include "../srcEngine/API.h"
#include "../srcEngine/glmPrint.hpp"
#include <set>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

mgeng::instanced* generateTestcube(mgeng::root *mgengRoot, float edgeLen, float width) {
	mgeng::instanced *ex = new mgeng::instanced(mgengRoot, /*nCols*/2, /*isOverlay*/false);
	float e = 0.5 * edgeLen;
	std::vector<glm::vec3> v1;
	v1.push_back(glm::vec3(-e, -e, -e));
	v1.push_back(glm::vec3(-e, e, -e));
	v1.push_back(glm::vec3(e, e, -e));
	v1.push_back(glm::vec3(e, -e, -e));
	std::vector<glm::vec3> v2;
	v2.push_back(glm::vec3(-e, -e, e));
	v2.push_back(glm::vec3(-e, e, e));
	v2.push_back(glm::vec3(e, e, e));
	v2.push_back(glm::vec3(e, -e, e));

	unsigned int nV = v1.size();
	assert(v2.size() == nV);
	unsigned int colIxOutline = 0;
	unsigned int colIxFill = 1;
	bool hitscanEnable = true;
	ex->generateOutlinedShape(v1, width, colIxOutline, colIxFill, hitscanEnable);
	ex->generateOutlinedShape(v2, width, colIxOutline, colIxFill, hitscanEnable);
	ex->generateOutlinedBody(v1, v2, width, colIxOutline, colIxFill, hitscanEnable);
	ex->finalize();
	return ex;
}

mgeng::instanced* generateCrosshairs(mgeng::root *mgengRoot) {
	mgeng::instanced *ex = new mgeng::instanced(mgengRoot, /*nCols*/1, /*isOverlay*/true);
	float a = 0.2f;
	float b = 5;
	std::vector<glm::vec3> vv;
	vv.push_back(glm::vec3(a, a, 0));
	vv.push_back(glm::vec3(a, b, 0));
	vv.push_back(glm::vec3(b, a, 0));

	float width = 0.15f;
	bool hitscanEnable = false;
	ex->generateOutlinedShape(vv, width, 0, /*invalid color*/999, hitscanEnable);

	for (auto it = vv.begin(); it != vv.end(); ++it)
		it->y *= -1;
	ex->generateOutlinedShape(vv, width, 0, /*invalid color*/999, hitscanEnable);

	for (auto it = vv.begin(); it != vv.end(); ++it)
		it->x *= -1;
	ex->generateOutlinedShape(vv, width, 0, /*invalid color*/999, hitscanEnable);

	for (auto it = vv.begin(); it != vv.end(); ++it)
		it->y *= -1;
	ex->generateOutlinedShape(vv, width, 0, /*invalid color*/999, hitscanEnable);
	ex->finalize();
	return ex;
}

class appObj: private mgeng::noncopyable {
public:
	appObj(mgeng::instanced *inst, std::vector<glm::vec3> *colDefault, std::vector<glm::vec3> *colHighlight) {
		this->inst = inst;
		this->colDefault = colDefault;
		this->colHighlight = colHighlight;
	}
	mgeng::instanced *inst;
	std::vector<glm::vec3> *colDefault;
	std::vector<glm::vec3> *colHighlight;
	glm::mat4 model2world;
	float explEndTime_s = 0;
	enum state_e {
		INACTIVE, ACTIVE, EXPLODING
	};
	state_e state = INACTIVE;
	mgeng::instancedExplosion *expl = NULL;
	mgeng::instancedExplosion* initExpl(mgeng::root *root, float explEndTime_s) {
		if (this->expl == NULL)
			this->expl = new mgeng::instancedExplosion();
		this->explEndTime_s = explEndTime_s;
		return this->expl;
	}
	~appObj() {
		delete this->expl;
	}
};
int main(void) {
	srand(0);
#ifdef NDEBUG
  std::cout << "NDEBUG set - assert disabled" << std::endl;
  assert(0);
#endif

	mgeng::root mgengRoot;
	mgengRoot.startup();

	mgeng::instanced *testcube = generateTestcube(&mgengRoot, 0.8f, 0.05f);
	mgeng::instanced *crosshairs = generateCrosshairs(&mgengRoot);

	mgeng::instancedExplosion expl;
	testcube->explode(&expl, glm::vec3(0, 0, 0), 0.5f, 1.0f);

	mgeng::observer *o = new mgeng::observer(&mgengRoot);
	o->setPitchYawRollKeys(GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT, 'Q', 'E');
	o->setMovementKeys(GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_PAGE_UP, GLFW_KEY_PAGE_DOWN);

	// === color scheme ===
	std::vector<glm::vec3> colDefault;
	colDefault.push_back(glm::vec3(0, 1, 0));
	colDefault.push_back(glm::vec3(0.1f, 0.1f, 0.1f));
	std::vector<glm::vec3> colHighlight;
	colHighlight.push_back(glm::vec3(0.4, 0.4, 0.4));
	colHighlight.push_back(glm::vec3(0.2f, 0.2f, 0.2f));
	std::vector<glm::vec3> colCrosshairs;
	colCrosshairs.push_back(glm::vec3(1, 1, 1));
	glm::mat4 projCrosshairs;

	std::set<appObj*> appObjs;
	for (float x = -10; x <= 10; ++x) {
		for (float z = -11; z <= -10; ++z) {
			appObj *o = new appObj(testcube, &colDefault, &colHighlight);
			o->model2world = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, z));
			o->inst = testcube;
			o->state = appObj::state_e::ACTIVE;
			appObjs.emplace(o);
		}
	}

	// === projection ===
	unsigned int screenWidth = 0;
	unsigned int screenHeight = 0;
	glm::mat4 persp;

	// === main loop ===
	float nextFpsTime = -1;
	while (true) {
		const mgeng::preDrawState *pds = mgengRoot.preDraw();
		if (pds->windowClose)
			break;

		// === update screen size ===
		// note: flag is always true on first frame (=> first preDraw)
		if (pds->screenSizeChanged) {
			mgengRoot.getScreenWidthHeight(screenWidth, screenHeight);
			persp = glm::perspective(45.0f, 1.0f * screenWidth / screenHeight, 0.01f, 1000.0f);
			projCrosshairs = persp * glm::translate (glm::mat4 (1.0f), glm::vec3 (0, 0, -50));
		}

		// === camera controls ===
		o->ctrlInput(pds);
		const glm::mat4 view = o->getView();
		glm::mat4 proj = persp * view;

		mgengRoot.beginDraw();
		glm::mat4 viewInv = glm::inverse(view);
		glm::vec4 viewerPos = viewInv * glm::vec4(0, 0, 0, 1);
		glm::vec4 viewAt = viewInv * glm::vec4(0, 0, -1, 1);

		bool mouseDown = mgengRoot.testKeycodePressEvt(mgeng::keycodeMouseButton0);
		glm::vec3 a = viewerPos;
		glm::vec3 b = viewAt - viewerPos;
		for (appObj *obj : appObjs) {
			switch (obj->state) {
			case appObj::state_e::ACTIVE: {
				// === check selection ===
				float dist = obj->inst->hitscan(obj->model2world, a, b);
				bool hit = !std::isnan(dist);
				glm::mat4 projT = proj * obj->model2world;
				obj->inst->render(projT, hit ? *obj->colHighlight : *obj->colDefault);
				if (hit & mouseDown) {
					obj->initExpl(&mgengRoot, pds->time_s + 3.0f);
					obj->inst->explode(obj->expl, glm::vec3(0, 0, 0), /*speed*/0.5f, /*angSpeed*/1.0f);
					obj->state = appObj::state_e::EXPLODING;
				}
				break;
			}
			case appObj::state_e::EXPLODING: {
				obj->inst->renderExplosion(obj->expl, proj * obj->model2world, glm::mat4(1.0f), *obj->colDefault); // TODO make pointer arg
				obj->expl->clock(pds->deltaTime_s);
				if (obj->explEndTime_s < pds->time_s)
					obj->state = appObj::state_e::INACTIVE;
				break;

			}
			default:
				break;
			}
		}

		crosshairs->render(projCrosshairs, colCrosshairs);

		mgengRoot.endDraw();
		if (pds->time_s > nextFpsTime) {
			float fps;
			mgengRoot.getFps(fps);
			std::cout << fps << " fps\n";
			nextFpsTime = pds->time_s + 1.0;
		}
	}
	mgengRoot.shutdown();
	std::cout << "main Done" << std::endl;
	return EXIT_SUCCESS;
}
