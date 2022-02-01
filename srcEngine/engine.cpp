#include "allMyGlIncludes.h"
#include <map>
#include <cassert>
#include "engine.h"
#include <math.h> // NAN
#include <iostream>
#define SINGLE_BUFFER
#define TILE_BASED
#define SUPERSAMPLING
#define WINDOWED
//#define NO_MOUSE
//#define ENABLE_RAW_MOUSE

#define SCANCODE_MB_FIRST (GLFW_KEY_LAST + 1) // first scancode used for mouse buttons

namespace engine {
engine::engine() {
	this->fps = NAN;
}

static std::map<GLFWwindow*, engine*> window2engine;

void engine::window_size_callback(GLFWwindow *window, int width, int height) {
	engine *e = window2engine[window];
	e->screenWidth = width;
	e->screenHeight = height;
	glViewport(0, 0, width, height);
}

// triggered by glfwPollEvents() in PRE_DRAW state
void engine::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
	engine *e = window2engine[window];
	e->pdsCurr.mouseX = xpos;
	e->pdsCurr.mouseY = ypos;
}

void engine::keyButtonCallback(int keycode, int action) {
	switch (action) {
	case GLFW_PRESS:
		this->keycodeDeltaDown.emplace(keycode);
		this->keycodeIsDown.emplace(keycode);
		break;
	case GLFW_RELEASE:
		this->keycodeDeltaUp.emplace(keycode);
		this->keycodeIsDown.erase(keycode);
		break;
	default: // GLFW_REPEAT
		break;
	}
}

void engine::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
	int scancode = button - GLFW_MOUSE_BUTTON_1 + SCANCODE_MB_FIRST;
	engine *e = window2engine[window];
	e->keyButtonCallback(button, action);
}

void engine::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	engine *e = window2engine[window];
	e->keyButtonCallback(key, action);
}

void engine::startup() {
	assert(this->state == NEW);
	this->state = READY;

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(1);
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef SINGLE_BUFFER
	glfwWindowHint( GLFW_DOUBLEBUFFER, GL_FALSE);
#else
	glfwWindowHint( GLFW_DOUBLEBUFFER, GL_TRUE);
#endif
#ifdef SUPERSAMPLING
	glfwWindowHint(GLFW_SAMPLES, 4);
#endif
// ignored for GLES glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); GLFW_CHK;

#ifdef WINDOWED
      this->window = glfwCreateWindow(this->screenWidth, this->screenHeight, __FILE__, NULL, NULL);
#else
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	this->screenWidth = mode->width;
	this->screenHeight = mode->height;
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	this->window = glfwCreateWindow(this->screenWidth, this->screenHeight, __FILE__, monitor, NULL);
#endif
	window2engine[this->window] = this;
	window_size_callback(this->window, this->screenWidth, this->screenHeight);

	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(this->window, window_size_callback);
	glfwSetCursorPosCallback(this->window, cursor_position_callback);
	glfwSetMouseButtonCallback(this->window, mouse_button_callback);
	glfwSetKeyCallback(this->window, key_callback);
#ifdef NO_MOUSE
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif

#ifdef ENABLE_RAW_MOUSE
    if (glfwRawMouseMotionSupported())
      glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
#endif

#ifndef linux
  // Windows version uses GLEW to load openGl libraries but this requires initialization
  // see also myGl.h
  glewExperimental = 1;// Needed for core profile
  if (glewInit () != GLEW_OK) {
    fprintf (stderr, "Failed to initialize GLEW\n");
    glfwTerminate ();
    exit (EXIT_FAILURE);
  }
#endif
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	printf("GL_VERSION  : %s\n", glGetString(GL_VERSION));
	printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER));
}

const mgeng::preDrawState* engine::preDraw() {
	assert(this->state == READY);
	this->state = PRE_DRAW;

	this->keycodeDeltaUp.clear();
	this->keycodeDeltaDown.clear();
	glfwPollEvents(); // invokes callbacks

// === calculate deltas ===
	this->pdsCurr.time_s = glfwGetTime();
	if (this->pdsCurr.frame > 0) {
		this->pdsCurr.deltaTime_s = this->pdsCurr.time_s - this->pdsPrev.time_s;
		this->pdsCurr.deltaMouseX = this->pdsCurr.mouseX - this->pdsPrev.mouseX;
		this->pdsCurr.deltaMouseY = this->pdsCurr.mouseY - this->pdsPrev.mouseY;
	}
	++this->pdsCurr.frame;
	this->pdsPrev = this->pdsCurr;

	this->pdsCurr.windowClose = glfwWindowShouldClose(this->window);

	this->fps = this->fpsEst.enterFrameGetFps(this->pdsCurr.time_s);
	return &this->pdsCurr;
}

bool engine::testKeycodePressed(int keycode) {
	return this->keycodeIsDown.find(keycode) != this->keycodeIsDown.end();
}

bool engine::testKeycodePressEvt(int keycode) {
	return this->keycodeDeltaDown.find(keycode) != this->keycodeDeltaDown.end();
}

bool engine::testKeycodeReleaseEvt(int keycode) {
	return this->keycodeDeltaUp.find(keycode) != this->keycodeDeltaUp.end();
}

void engine::beginDraw() {
	assert(this->state == PRE_DRAW);
	this->state = DRAW;

#ifndef TILE_BASED
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
// https://www.seas.upenn.edu/~pcozzi/OpenGLInsights/OpenGLInsights-TileBasedArchitectures.pdf
	glDisable(GL_SCISSOR_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilMask(0xFFFFFFFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#endif
}

void engine::endDraw() {
	assert(this->state == DRAW);
	this->state = READY;
// === show new image ===
#ifdef SINGLE_BUFFER
	glFlush();
#else
	glfwSwapBuffers(this->window);
#endif
}

void engine::shutdown() {
	assert(this->state != NEW); // need glfwInit()
	this->state = SHUTDOWN;
	glfwTerminate();
}
} // namespace

// ==============================================================
// == API
// ==============================================================
mgeng::root::root(){
	this->eng = new engine::engine();
}

mgeng::root::~root(){
	delete this->eng;
}

void mgeng::root::beginDraw() {
	this->eng->beginDraw();
}

void mgeng::root::endDraw() {
	this->eng->endDraw();
}

const mgeng::preDrawState* mgeng::root::preDraw() {
	return this->eng->preDraw();
}

void mgeng::root::startup() {
	this->eng->startup();
}

void mgeng::root::getScreenWidthHeight(unsigned int& width, unsigned int& height){
	width = this->eng->screenWidth;
	height = this->eng->screenHeight;
}

void mgeng::root::shutdown() {
	this->eng->shutdown();
}

void mgeng::root::getFps(float& fps){
	fps = this->eng->fps;
}
