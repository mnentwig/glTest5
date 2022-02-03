#pragma once
#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef signed char GLbyte;
typedef short GLshort;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned long GLulong;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;
typedef char GLchar;

// opaque payload for API wrapper
namespace engine {
class engine;
class observer;
class explosible;
class explTraj;
}
// incomplete classes for "friend"
namespace mgeng {
class observer;
class instanced;
class instancedExplosion;

class noncopyable {
protected:
	constexpr noncopyable() = default;
	~noncopyable() = default;
	noncopyable(const noncopyable&) = delete;
	noncopyable& operator=(const noncopyable&) = delete;
};

// glTypes here...
struct triIx16 {
	GLushort v1;
	GLushort v2;
	GLushort v3;
};

class preDrawState {
public:
	float time_s;
	float deltaTime_s = 0;

	float mouseX;
	float deltaMouseX = 0;
	float mouseY;
	float deltaMouseY = 0;

	bool windowClose;
	bool mouseInWindow = true;

	long frame = 0;
	bool screenSizeChanged;
};

class root: private noncopyable {
public:
	root();
	~root();
	void startup();
	void getScreenWidthHeight(unsigned int &width, unsigned int &height);
	void getFps(float &fps);
	const preDrawState* preDraw();
	void beginDraw();
	void endDraw();
	void shutdown();
    bool testKeycodePressed(int keycode);
    bool testKeycodePressEvt(int keycode);
    bool testKeycodeReleaseEvt(int keycode);
protected:
	engine::engine *eng;
	friend observer;
	friend instanced;
};

class observer: private noncopyable {
public:
	explicit observer(root*);
	~observer();
	void setPitchYawRollKeys(int pitchDown, int pitchUp, int YawLeft, int YawRight, int rollCcw, int rollCw);
	void setMovementKeys(int forwards, int backwards, int left, int right, int up, int down);
	void ctrlInput(const mgeng::preDrawState *pds);
	const glm::mat4x4& getView() const;

protected:
	engine::observer *obs;
};

class instanced: private noncopyable {
public:
	instanced(root*, unsigned int nCols, bool isOverlay);
	~instanced();
	void generateOutlinedShape(std::vector<glm::vec3> vertices, float width, unsigned int ixColOutline, unsigned int ixColFill, bool hitscanEnable);
	void generateOutlinedBody(std::vector<glm::vec3> vertices1, std::vector<glm::vec3> vertices2, float width, unsigned int ixColOutline, unsigned int ixColFill, bool hitscanEnable);
	void finalize();
	void render(const glm::mat4& proj, const std::vector<glm::vec3>& col);
	void explode(mgeng::instancedExplosion *traj, glm::vec3 impact, float speed, float angSpeed);
	void renderExplosion(const instancedExplosion* traj, const glm::mat4 &model2screen, const glm::mat4 &model2model, const std::vector<glm::vec3> &rgb);
	float hitscan(const glm::mat4 &proj, const glm::vec3 &lineOrig, const glm::vec3 &lineDir) const;
protected:
	engine::explosible *ex;
};

class instancedExplosion: private noncopyable {
public:
	instancedExplosion();
	void clock(float deltaT_s);
protected:
	engine::explTraj *traj;
	friend instanced;
};

constexpr unsigned int keycodeMouseButton0 = 0x1000;
} // namespace
