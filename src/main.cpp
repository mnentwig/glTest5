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
	mgeng::instanced *ex = new mgeng::instanced(mgengRoot, 2);
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

class appObj: private mgeng::noncopyable{
public:
	appObj(mgeng::instanced* inst, std::vector<glm::vec3>* colDefault, std::vector<glm::vec3>* colHighlight){
		this->inst=inst;
		this->colDefault = colDefault;
		this->colHighlight = colHighlight;
	};
	mgeng::instanced* inst;
	std::vector<glm::vec3>* colDefault;
	std::vector<glm::vec3>* colHighlight;
	glm::mat4 model2world;
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

	mgeng::instancedExplosion expl(&mgengRoot);
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

	std::set<appObj*> appObjs;
	for (float x = -10; x <= 10; ++x) {
		for (float z = -11; z <= -10; ++z) {
			appObj* o = new appObj(testcube, &colDefault, &colHighlight);
			o->model2world = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, z));
			o->inst = testcube;
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
		}

		// === camera controls ===
		o->ctrlInput(pds);
		const glm::mat4 view = o->getView();
		glm::mat4 proj = persp * view;

		mgengRoot.beginDraw();
		glm::mat4 viewInv = glm::inverse(view);
		glm::vec4 viewerPos = viewInv * glm::vec4(0,0,0, 1);
		glm::vec4 viewAt = viewInv * glm::vec4(0,0,-1, 1);
//glmPrint(viewerPos);

		//float phi = pds->time_s * 2.0 * M_PI / 4;
		//glm::mat4 rot = glm::rotate(glm::mat4(1.0f), phi, glm::vec3(0, 1, 0));
		for (appObj* obj:appObjs){
			obj->inst->render(proj*obj->model2world, *obj->colDefault); // TODO make pointer arg
		}
		//testcube->renderExplosion(&expl, projT, glm::mat4(1.0f), col);
		//expl.clock(pds->deltaTime_s);

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
