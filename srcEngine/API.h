#pragma once
#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

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

namespace engine {
class engine;
class observer;
}
namespace mgeng {
class observer;

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
};

class root {
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
protected:
	engine::engine *eng;
	root(const root&) = delete; // FORBID COPY
	root& operator =(const root&) = delete;  // FORBID COPY
	friend observer;
};

class observer {
public:
	explicit observer(root*);
	~observer();
	void setPitchYawRollKeys(int pitchDown, int pitchUp, int YawLeft, int YawRight, int rollCcw, int rollCw);
	void setMovementKeys(int forwards, int backwards, int left, int right, int up, int down);
	void ctrlInput(const mgeng::preDrawState *pds);
	const glm::mat4x4& getView() const;

protected:
	engine::observer *obs;
	observer(const observer&) = delete; // FORBID COPY
	observer& operator =(const observer&) = delete;  // FORBID COPY
};
#if 0
class instancedObj {
public:
	instancedObj(root*, int nCols);
	~instancedObj();
};
#endif
} // namespace
