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
	blueprint(instMan *im, unsigned int nCol);
	void finalize();
	void render(const glm::mat4 &proj, const std::vector<glm::vec3> &rgb);
protected:
	instMan *im;
	unsigned int imHandle;
	blueprint(const blueprint&) = delete; // prevent copy
	blueprint& operator =(const blueprint&) = delete; // prevent copy
};

class blueprintHitscan: public blueprint {
public:
	blueprintHitscan(instMan *im, unsigned int nCol);
	void addHitscanSurface(const std::vector<glm::vec3> vertices);
	float lineIntersectAtDistance(const glm::mat4 &proj, const glm::vec3 &lineOrig, const glm::vec3 &lineDir);
protected:
	typedef struct {
	public:
		glm::vec3 p0;
		glm::vec3 p1;
		glm::vec3 p2;
	} collisionTri;
	std::vector<collisionTri> collisionTriList;
	std::vector<std::vector<glm::vec3>> hitscanSurfaces;
};

class explosible: public blueprintHitscan {
public:
	explosible(instMan *im, unsigned int nCol);
	void generateOutlinedShape(std::vector<glm::vec3> vertices, float width);
	void generateOutlinedBody(std::vector<glm::vec3> vertices1, std::vector<glm::vec3> vertices2, float width);
	void closeFragment();
	void renderExplosion(const glm::mat4 &model2model, const glm::mat4 &model2world, const explTraj &traj, const std::vector<glm::vec3> &rgb);
	void explode(explTraj *traj, glm::vec3 impact, float speed, float angSpeed);
	bool lineIntersectCheck(const glm::mat4 &proj, const glm::vec3 &pos, const glm::vec3 &delta, float &distLimit) const;
protected:
	fragment *currentFragment;
	std::vector<fragment*> fragments;
};
