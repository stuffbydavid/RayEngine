#include "rayengine.h"

RayEngine::RayEngine(int windowWidth, int windowHeight, RenderMode renderMode, RayTracingTarget rayTracingTarget, float hybridPartition) :
	renderMode(renderMode),
	rayTracingTarget(rayTracingTarget),
	hybridPartition(hybridPartition),
	showEmbreeRender(true),
	showOptixRender(true)
{

	window.init(windowWidth, windowHeight);
	Magick::InitializeMagick(NULL);
	
	// Shaders
	shdrNormals = new Shader("Normals", bind(&RayEngine::setupNormals, this, _1, _2, _3), "normals.vshader", "normals.fshader");
	shdrTexture = new Shader("Texture", bind(&RayEngine::setupTexture, this, _1, _2, _3), "texture.vshader", "texture.fshader");
	shdrPhong = new Shader("Phong", bind(&RayEngine::setupPhong, this, _1, _2, _3), "phong.vshader", "phong.fshader");

}

RayEngine::~RayEngine() {
	// TODO
}

void RayEngine::launch() {

	initEmbree();
	initOptix();

	window.open(bind(&RayEngine::loop, this), bind(&RayEngine::resize, this));

}

void RayEngine::loop() {

	input();
	
	string mode = "";

	if (renderMode == RM_OPENGL) {
		renderOpenGL();
		mode = "OpenGL";
	} else if (rayTracingTarget == RTT_CPU) {
		renderEmbree();
		renderEmbreeTexture();
		mode = "Embree";
	} else if (rayTracingTarget == RTT_GPU) {
		renderOptix();
		renderOptixTexture();
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

	curScene = new Scene(name);
	curCamera = &curScene->camera;
	scenes.push_back(curScene);
	return curScene;

}