#include "rayengine.h"

RayEngine::RayEngine(int windowWidth, int windowHeight, RenderMode renderMode, RayTracingTarget rayTracingTarget) :
    renderMode(renderMode),
	rayTracingTarget(rayTracingTarget)
{

	window.init(windowWidth, windowHeight);

	// Init libraries
	Magick::InitializeMagick(NULL);
	initEmbree();
	initOptix();

	// Shaders
	shdrOGL = new Shader("OpenGL", Shader::setupOGL, "ogl.vshader", "ogl.fshader");

}

RayEngine::~RayEngine() {

}

void RayEngine::launch() {

	// Init Embree
	for (uint i = 0; i < scenes.size(); i++)
		scenes[i]->root.initEmbree(EmbreeData.device);

	window.open(bind(&RayEngine::update, this));

}

void RayEngine::update() {

	input();

	if (renderMode == RM_OPENGL)
		renderOpenGL();
	else if (rayTracingTarget == RTT_CPU)
		renderEmbree();
	else if (rayTracingTarget == RTT_GPU)
		renderOptix();
	else if (rayTracingTarget == RTT_HYBRID)
		renderHybrid();

	window.setTitle("RayEngine - FPS: " + to_string(window.fps));

}

Scene* RayEngine::createScene(string name) {

	Scene* scene = new Scene(name);
	scenes.push_back(scene);
	return scene;

}