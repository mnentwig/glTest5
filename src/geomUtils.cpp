#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/fwd.hpp>
#include <glm/gtx/norm.hpp>
#include <algorithm> // std::max

class geomUtils {
	/** Return minimum distance between line segment vw and point p */
	static float minimum_distance(const glm::vec2& v, const glm::vec2& w, const glm::vec2& p) {
		float l2 = glm::distance2(v, w);  // i.e. |w-v|^2 -  avoid a sqrt
		if (l2 == 0.0)
			return glm::distance(p, v);   // v == w case
		// Consider the line extending the segment, parameterized as v + t (w - v).
		// We find projection of point p onto the line.
		// It falls where t = [(p-v) . (w-v)] / |w-v|^2
		// We clamp t from [0,1] to handle points outside the segment vw.
		float t = std::max(0.0f, std::min(1.0f, glm::dot(p - v, w - v) / l2));
		glm::vec2 projection = v + t * (w - v); // Projection falls on the segment
		return glm::distance(p, projection);
	}
};
