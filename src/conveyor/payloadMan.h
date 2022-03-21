#pragma once
#include "srcEngine/API_fwd.h"
#include <vector>
#include <glm/vec3.hpp>
namespace game {
class payload;
class payloadMan {
public:
	payloadMan(mgeng::root *mgengRoot, std::vector<glm::vec3> colorscheme);
	payload* payload_new();
protected:
	mgeng::instanced *inst;
};
} // namespace
