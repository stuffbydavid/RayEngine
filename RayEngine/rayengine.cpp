#include "rayengine.h"

RayEngine::RayEngine(int windowWidth, int windowHeight, RenderMode renderMode, RayTracingTarget rayTracingTarget) :
    renderMode(renderMode),
	rayTracingTarget(rayTracingTarget)
{

	window.init(windowWidth, windowHeight);

	// Init libraries
	Magick::InitializeMagick(NULL);
	embreeHandler.init();
	optixHandler.init();

}

RayEngine::~RayEngine() {

}

void RayEngine::launch() {

	window.open(bind(&RayEngine::update, this));

}

void RayEngine::update() {

	window.setTitle("RayEngine - FPS: " + to_string(window.fps));

}

Scene* RayEngine::createScene(string name) {

	Scene* scene = new Scene(name);
	scenes.push_back(scene);
	return scene;

}