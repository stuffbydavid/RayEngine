#pragma once

#include "common.h"
#include "settings.h"
#include "window.h"
#include "scene.h"
#include "shader.h"
#include "font.h"

struct RayEngine {

	// Render modes
	enum RenderMode {
		RM_RAY_TRACING, // Render using the ray tracing algorithm
		RM_OPENGL // Render using OpenGL shaders
	};

	// Ray tracing targets
	enum RayTracingTarget {
		RTT_CPU, // Use Embree for CPU ray tracing
		RTT_GPU, // Use OptiX for GPU ray tracing
		RTT_HYBRID // Use both Embree and OptiX for CPU and GPU ray tracing
	};

	// Constructor
	RayEngine(int windowWidth = WINDOW_WIDTH,
			  int windowHeight = WINDOW_HEIGHT,
			  RenderMode renderMode = RENDER_MODE,
			  RayTracingTarget rayTracingTarget = RAY_TRACING_TARGET,
			  float hybridPartition = HYBRID_PARTITION);

	~RayEngine();

	// Adds a new, empty scene.
	Scene* createScene(string name, Color ambient = { 0.f }, string skyFile = "", Color skyColor = { 0.f });

	// Launches the program and starts rendering.
	void launch();

	// Called by the window.
	void loop();
	void resize();
	void input();

	// Variables
	vector<Scene*> scenes;
	Scene* curScene;
	Camera* curCamera;
	Window window;
	RenderMode renderMode;
	RayTracingTarget rayTracingTarget;
	Vec3 rayOrg, rayXaxis, rayYaxis, rayZaxis;

	// GUI
	void GUIRender();
	Font* fntGUI;

	// OpenGL
	void openglRender();
	void openglSetupNormals(GLuint program, Object* object, TriangleMesh* mesh);
	void openglSetupPhong(GLuint program, Object* object, TriangleMesh* mesh);
	Shader* shdrColor;
	Shader* shdrTexture;
	Shader* shdrNormals;
	Shader* shdrPhong;

	// Embree
	struct EmbreeData{

		RTCDevice device;
		vector<Color> buffer;
		GLuint texture;
		int offset, width, frames;
		float time, avgTime;

	} EmbreeData;
	void embreeInit();
	void embreeResize();
	void embreeRender();
	void embreeRenderFirePrimaryRay(int x, int y);
	void embreeRenderFirePrimaryPacket(int x, int y);
	void embreeRenderTraceRay(RTCRay& ray, int depth, Color& result);
	void embreeRenderTracePacket(RTCRay8& packet, int* valid, int depth, Color* result);
	void embreeRenderUpdateTexture();
	Color embreeRenderSky(Vec3 dir);

	// OptiX
	struct OptixData  {

		optix::Context context;
		optix::Buffer renderBuffer, lights;
		GLuint vbo;
		GLuint texture;
		int offset, width, frames;
		float time, avgTime;

	} OptixData;
	void optixInit();
	void optixResize();
	void optixRender();
	void optixRenderUpdateTexture();

	// Hybrid
	void hybridRender();
	float hybridPartition;

};