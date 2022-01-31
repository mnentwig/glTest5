#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include <GLFW/glfw3.h>
#include "../srcEngine/engine.h"
#include "../srcEngine/observer.h"
#include "../srcEngine/instMan.h"
#include "../srcEngine/explosible.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include "../srcEngine/glTypes.h"

explosible* generateTestcube(float edgeLen, float width, instMan *im) {
	explosible *ex = new explosible(im);
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

	ex->generateOutlinedShape(v1.data(), nV, width);
	ex->generateOutlinedShape(v2.data(), nV, width);
	ex->generateOutlinedBody(v1.data(), v2.data(), nV, width);
	ex->finalize();
	return ex;
}

int main(void) {
	instMan im;
	srand(0);
#ifdef NDEBUG
  std::cout << "NDEBUG set - assert disabled" << std::endl;
  assert(0);
#endif

	engine::engine e;
	e.startup();

	explosible *testcube = generateTestcube(0.8f, 0.05f, &im);
	engine::observer o(&e);
	o.setPitchYawRollKeys(GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT, 'Q', 'E');
	o.setMovementKeys(GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_PAGE_UP, GLFW_KEY_PAGE_DOWN);
	glm::mat4 persp = glm::perspective(45.0f, 1.0f * e.screenWidth / e.screenHeight, 0.01f, 1000.0f);
	while (true) {
		const engine::preDrawState *pds = e.preDraw();
		if (pds->windowClose)
			break;
		o.ctrlInput(pds);

		e.beginDraw();
		const glm::mat4 view = o.getView();

		glm::mat4 proj = persp * view;

		im.startFrame();
		glm::vec3 rgbOuter(0, 1, 0);
		glm::vec3 rgbInner(0.1, 0.1, 0.1);
		float phi = pds->time_s * 2.0 * M_PI / 4;

		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), phi, glm::vec3(0, 1, 0));

		for (float x = -10; x <= 10; ++x) {
			for (float z = -11; z <= -10; ++z) {
				glm::mat4 v = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, z)) * rot;
				glm::mat4 projT = proj * v;
				testcube->render(projT, rgbOuter, rgbInner);
			}
		}

		im.endFrame();
		e.endDraw();
		if (pds->frame % 30 == 0)
			std::cout << e.fps << " fps\n";
	}
	im.shutdown();
	e.shutdown();
	std::cout << "main Done" << std::endl;
	return EXIT_SUCCESS;
}
