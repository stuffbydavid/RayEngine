#pragma once

#include "common.h"
#include "window.h"
#include "embree_handler.h"
#include "optix_handler.h"
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
			  RayTracingTarget rayTracingTarget = RTT_CPU);

	~RayEngine();

	// Adds a new scene
	Scene* createScene(string name);

	// Launches the program and starts rendering
	void launch();

private:

	vector<Scene*> scenes;

	EmbreeHandler embreeHandler;
	OptixHandler optixHandler;

	Window window;
	RenderMode renderMode;
	RayTracingTarget rayTracingTarget;

	void update();

};