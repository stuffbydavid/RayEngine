#include "rayengine.h"


void setupNormals(GLuint program, void* caller) {

	GLint uMatWorld = glGetUniformLocation(program, "uMatWorld");
	glUniformMatrix4fv(uMatWorld, 1, GL_FALSE, Mat4x4(((Object*)caller)->matrix).e);

}

void setupTexture(GLuint program, void* caller) {

	GLint uColor = glGetUniformLocation(program, "uColor");
	glUniform4f(uColor, 1.f, 1.f, 1.f, 1.f);

}

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
	shdrNormals = new Shader("Normals", setupNormals, "normals.vshader", "normals.fshader");
	shdrTexture = new Shader("Texture", setupTexture, "texture.vshader", "texture.fshader");

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