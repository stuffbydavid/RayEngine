#pragma once

#include "common.h"
#include "settings.h"
#include "window.h"
#include "scene.h"
#include "shader.h"

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

	// OpenGL
	void openglRender();
	void openglSetupNormals(GLuint program, Object* object, TriangleMesh* mesh);
	void openglSetupTexture(GLuint program, Object* object, TriangleMesh* mesh);
	void openglSetupPhong(GLuint program, Object* object, TriangleMesh* mesh);
	Shader* shdrNormals;
	Shader* shdrTexture;
	Shader* shdrPhong;

	// Embree
	struct EmbreeData{

		RTCDevice device;
		vector<Color> buffer;
		GLuint texture;
		int offset, width, frames;
		float time, avgTime;

		// Stores a ray
		struct Ray {
			int x, y;
			Vec3 org, dir;
			float factor;
			Color result;
		};

		// Stores the properties of a ray hit
		struct RayHit {
			Color diffuse, specular;
			Vec3 pos, normal;
			Vec2 texCoord;
			Object* obj;
			TriangleMesh* mesh;
			Material* material;
		};

		// Stores a light ray
		struct LightRay {
			Vec3 incidence;
			float distance, attenuation;
			Color lightColor;
		};

		// Stores a packet of rays
		template <typename R> struct Packet {
			R rays[EMBREE_PACKET_SIZE];
			EMBREE_PACKET_TYPE ePacket;
			__aligned(32) int valid[EMBREE_PACKET_SIZE];
		};

		typedef Packet<Ray> RayPacket;
		typedef Packet<LightRay> LightRayPacket;

	} EmbreeData;
	void embreeInit();
	void embreeResize();
	void embreeRender();
	void embreeRenderTiles();
	void embreeRenderSingleLoop();
	void embreeRenderTracePacket(EmbreeData::RayPacket& packet, int depth);
	Color embreeRenderSky(Vec3 dir);
	void embreeRenderUpdateTexture();

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