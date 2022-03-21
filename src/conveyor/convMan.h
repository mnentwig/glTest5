#pragma once
#include <set>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
namespace mgeng {
class instanced;
class root;
}
namespace game {

class convSeg;
/// encapsulates the API for conveyor functionality
class convMan {
public:
	convMan(mgeng::root *mgengRoot, std::vector<glm::vec3> colorscheme);
	~convMan();
	convSeg* newSeg_straight(glm::mat4 &model2world);
	void connect(convSeg *sFirst, convSeg *sSecond);
	void render(const glm::mat4 &proj);
protected:
	mgeng::instanced *straight;
	std::set<convSeg*> allConvSegs;
	std::vector<glm::vec3> colorscheme;
}/*class*/;
}
