class instMan;
//#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
class instStackTriInst;
class fragment;
class explTraj;

class blueprint {
public:
	explicit blueprint(instMan *im, unsigned int nCol);
	void finalize();
	void render(const glm::mat4 &proj, const std::vector<glm::vec3> &rgb);
protected:
	instMan *im;
	unsigned int imHandle;
	blueprint(const blueprint&) = delete; // prevent copy
	blueprint& operator =(const blueprint&) = delete; // prevent copy
};

class explosible: public blueprint {
public:
	explicit explosible(instMan *im, unsigned int nCol);
	void generateOutlinedShape(glm::vec3 *vertices, unsigned int nVertices, float width);
	void generateOutlinedBody(glm::vec3 *vertices1, glm::vec3 *vertices2, unsigned int nVertices, float width);
	void closeFragment();
	void renderExplosion(const glm::mat4 &model2model, const glm::mat4 &model2world, const explTraj &traj, const std::vector<glm::vec3> &rgb);
	void explode(explTraj *traj, glm::vec3 impact, float speed, float angSpeed);
	bool lineIntersectCheck(const glm::mat4 &proj, const glm::vec3 &pos, const glm::vec3 &delta, float &distLimit) const;
protected:
	fragment *currentFragment;
	std::vector<fragment*> fragments;
};
