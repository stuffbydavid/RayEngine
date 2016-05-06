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
		int offset, width;

		// Stores a packet of rays
		struct RayPacket {
			int x, y;
		};

		// Stores a radiance ray
		struct Ray {
			int x, y;
			Vec3 pos, dir;
			float factor;
		};

		// Stores a ray hit
		struct RayHit {
			int x, y;
			Color diffuse, specular;
			Vec3 pos;
			Object* obj;
			TriangleMesh* mesh;
			Material* material;
			Vec3 normal;
			Vec2 texCoord;
			float factor;
		};

		// Stores a shadow ray
		struct ShadowRay {
			int hitID;
			Color lightColor;
			Vec3 incidence;
			float distance, attenuation;
		};
			
	} EmbreeData;
	void embreeInit();
	void embreeResize();
	void embreeRender();
	void embreeRenderOld();
	Color embreeRenderProcessRay(RTCRay& ray, int depth);
	void embreeRenderTiles();
	void embreeRenderProcessPacket(EmbreeData::RayPacket& packet, int depth);
	void embreeRenderLists();
	void embreeRenderProcessList(vector<EmbreeData::Ray>& rays, int depth);
	Color embreeRenderSky(Vec3 dir);
	void embreeRenderUpdateTexture();

	// OptiX
	struct OptixData  {
		optix::Context context;
		optix::Buffer renderBuffer, lights;
		GLuint vbo;
		GLuint texture;
		int offset, width;
	} OptixData;
	void optixInit();
	void optixResize();
	void optixRender();
	void optixRenderUpdateTexture();

	// Hybrid
	void hybridRender();
	float hybridPartition;

};