#include "rayengine.h"

RayEngine::RayEngine(int windowWidth, int windowHeight, RenderMode renderMode, RayTracingTarget rayTracingTarget, float hybridPartition) :
	renderMode(renderMode),
	rayTracingTarget(rayTracingTarget),
	hybridPartition(hybridPartition)
{

	window.init(windowWidth, windowHeight);
	Magick::InitializeMagick(NULL);

	// Font
	FT_Library freeType;
	if (FT_Init_FreeType(&freeType))
		cout << "Could not init freetype library!";
	fntGUI = new Font(&freeType, "font/tahoma.ttf", 32, 128, 12);

	// Shaders
	shdrColor = new Shader("Color", nullptr, "texture.vshader", "texture.fshader");
	shdrTexture = new Shader("Texture", nullptr, "texture.vshader", "texture.fshader");
	shdrNormals = new Shader("Normals", bind(&RayEngine::openglSetupNormals, this, _1, _2, _3), "normals.vshader", "normals.fshader");
	shdrPhong = new Shader("Phong", bind(&RayEngine::openglSetupPhong, this, _1, _2, _3), "phong.vshader", "phong.fshader");

	hybridDirection = -1.f;

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
	
	rayOrg = curCamera->position;
	rayXaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
	rayYaxis = curCamera->yaxis * curCamera->tFov;
	rayZaxis = curCamera->zaxis;

	string mode = "";
	if (renderMode == RM_OPENGL) {
		mode = "OpenGL";
		openglRender();
	} else if (rayTracingTarget == RTT_CPU) {
		mode = "Embree";
		embreeRender();
		embreeRenderUpdateTexture();
	} else if (rayTracingTarget == RTT_GPU) {
		mode = "OptiX";
		optixRender();
		optixRenderUpdateTexture();
	} else if (rayTracingTarget == RTT_HYBRID) {
		mode = "Hybrid";
		hybridRender();
		hybridUpdatePartition();
	}

	window.setTitle("RayEngine - " + mode +
					" - FPS: " + to_string(window.fps) +
					", Embree avg: " + to_string(EmbreeData.avgTime) +
					", Optix avg: " + to_string(OptixData.avgTime));


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