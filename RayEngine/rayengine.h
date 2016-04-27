#pragma once

#include "common.h"
#include "window.h"
#include "scene.h"

struct RayEngine {

	// Render modes
	enum RenderMode {
		RM_RAYTRACING, // Render using the ray tracing algorithm
		RM_OPENGL // Render using OpenGL shaders
	};

	// Ray tracing targets
	enum RayTracingTarget {
		RTT_CPU, // Use Embree for CPU ray tracing
		RTT_GPU, // Use OptiX for GPU ray tracing
		RTT_HYBRID // Use both Embree and OptiX for CPU and GPU ray tracing
	};

	RayEngine(int windowWidth = 900,
			  int windowHeight = 600,
			  RenderMode renderMode = RM_RAYTRACING,
			  RayTracingTarget rayTracingTarget = RTT_GPU,
			  float hybridPartition = 0.5);

	~RayEngine();

	// Adds a new, empty scene.
	Scene* createScene(string name);

	// Launches the program and starts rendering.
	void launch();

private:

	// Embree
	struct {
		RTCDevice device;
		GLuint texture;
		Color* buffer;
	} EmbreeData;
	void initEmbree();

	// OptiX
	struct {
		optix::Context context;
		optix::Buffer buffer;
		GLuint vbo;
		GLuint texture;
	} OptixData;
	void initOptix();

	vector<Scene*> scenes;
	Window window;
	RenderMode renderMode;
	RayTracingTarget rayTracingTarget;
	float hybridPartition;
	Shader* shdrOGL;

	void update();
	void resize();
	void resizeEmbree();
	void resizeOptix();
	void renderOpenGL();
	void renderEmbree();
	void renderOptix();
	void renderHybrid();
	void input();

};