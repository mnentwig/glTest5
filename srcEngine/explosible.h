class instMan;
//#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
class instStackTriInst;
class fragment;
class explTraj;
class explosible {
 public:
  explosible(instMan* im);
  void generateOutlinedShape(glm::vec3* vertices, unsigned int nVertices, float width);
  void generateOutlinedBody(glm::vec3* vertices1, glm::vec3* vertices2, unsigned int nVertices, float width);
  void closeFragment();
  void finalize();
  void render(const glm::mat4& proj, const glm::vec3& rgbOuter, const glm::vec3& rgbInner);
  void renderExplosion(const glm::mat4 &model2model, const glm::mat4& model2world,
		  const explTraj &traj, const glm::vec3 &rgbOuter, const glm::vec3 &rgbInner);
  void explode(explTraj* traj, glm::vec3 impact, float speed, float angSpeed);
  bool lineIntersectCheck(const glm::mat4& proj, const glm::vec3& pos, const glm::vec3& delta, float& distLimit) const;
  explosible(const explosible&) = delete; // prevent copy
  explosible& operator =(const explosible&) = delete; // prevent copy
 protected:
  instMan* im;
  unsigned int imHandle;
  instStackTriInst* isOutline;
  instStackTriInst* isFill;
  fragment* currentFragment;
  std::vector<fragment*> fragments;
};
