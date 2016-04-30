#include "rayengine.h"


void setupOGL(GLuint program, void* caller) {

	GLint uMatWorld = glGetUniformLocation(program, "uMatWorld");
	glUniformMatrix4fv(uMatWorld, 1, GL_FALSE, Mat4x4(((Object*)caller)->matrix).e);

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
	shdrOGL = new Shader("OpenGL", setupOGL, "ogl.vshader", "ogl.fshader");
	shdrTex = new Shader("Texture", nullptr, "tex.vshader", "tex.fshader");

}

RayEngine::~RayEngine() {

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