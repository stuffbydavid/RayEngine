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
	shdrNormals = new Shader("Normals", bind(&RayEngine::openglSetupNormals, this, _1, _2, _3), "normals.vshader", "normals.fshader");
	shdrTexture = new Shader("Texture", bind(&RayEngine::openglSetupTexture, this, _1, _2, _3), "texture.vshader", "texture.fshader");
	shdrPhong = new Shader("Phong", bind(&RayEngine::openglSetupPhong, this, _1, _2, _3), "phong.vshader", "phong.fshader");

}

RayEngine::~RayEngine() {
	// TODO
}

void RayEngine::launch() {

	embreeInit();
	optixInit();

	window.open(bind(&RayEngine::loop, this), bind(&RayEngine::resize, this));

}

void RayEngine::loop() {

	input();
	
	string mode = "";

	if (renderMode == RM_OPENGL) {
		openglRender();
		mode = "OpenGL";
	} else if (rayTracingTarget == RTT_CPU) {
		embreeRender();
		embreeRenderUpdateTexture();
		mode = "Embree";
	} else if (rayTracingTarget == RTT_GPU) {
		optixRender();
		optixRenderUpdateTexture();
		mode = "OptiX";
	} else if (rayTracingTarget == RTT_HYBRID) {
		hybridRender();
		mode = "Hybrid";
	}

	window.setTitle("RayEngine - " + mode + " - FPS: " + to_string(window.fps));

}

void RayEngine::resize() {

	embreeResize();
	optixResize();

}

Scene* RayEngine::createScene(string name, Color ambient, string skyFile, Color background) {

	curScene = new Scene(name, ambient, skyFile, background);
	curCamera = &curScene->camera;
	scenes.push_back(curScene);
	return curScene;

}