#include "rayengine.h"

RayEngine::RayEngine(int windowWidth, int windowHeight, RenderMode renderMode, RayTracingTarget rayTracingTarget, float hybridPartition) :
    renderMode(renderMode),
	rayTracingTarget(rayTracingTarget),
	hybridPartition(hybridPartition)
{

	window.init(windowWidth, windowHeight);
	Magick::InitializeMagick(NULL);

	// Shaders
	shdrOGL = new Shader("OpenGL", Shader::setupOGL, "ogl.vshader", "ogl.fshader");

}

RayEngine::~RayEngine() {

}

void RayEngine::launch() {

	initEmbree();
	initOptix();

	window.open(bind(&RayEngine::update, this), bind(&RayEngine::resize, this));

}

void RayEngine::update() {

	input();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, window.width, window.height);

	glClearColor(0.1, 0.1, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	string mode = "";

	if (renderMode == RM_OPENGL) {
		renderOpenGL();
		mode = "OpenGL";
	} else if (rayTracingTarget == RTT_CPU) {
		renderEmbree();
		mode = "Embree";
	} else if (rayTracingTarget == RTT_GPU) {
		renderOptix();
		mode = "OptiX";
	} else if (rayTracingTarget == RTT_HYBRID) {
		renderHybrid();
		mode = "Hybrid";
	}

	window.setTitle("RayEngine - " + mode + " - FPS: " + to_string(window.fps));

}

void RayEngine::resize() {

	resizeEmbree();
	resizeOptix();

}

Scene* RayEngine::createScene(string name) {

	Scene* scene = new Scene(name);
	scenes.push_back(scene);
	return scene;

}