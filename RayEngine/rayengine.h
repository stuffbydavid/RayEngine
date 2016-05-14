#pragma once

#include "common.h"
#include "settings.h"
#include "window.h"
#include "scene.h"
#include "shader.h"
#include "font.h"

struct RayEngine {

	RayEngine();
	~RayEngine();

	// Launches the program and starts rendering.
	void launch();

	//// Window ////

	Window window;
	void loop();
	void resize();

	//// Scene ////

	vector<Scene*> scenes;
	int selectedScene;
	Scene* curScene;
	Camera* curCamera;
	Vec3 rayOrg, rayXaxis, rayYaxis, rayZaxis;

	Scene* createScene(string name, string skyFile = "", Color ambient = { 0.f }, float aoRadius = 5.f, Color skyColor = { 0.f });
	void cameraInput();

	//// Settings ////

	enum RenderMode {
		RM_OPENGL, // Render using OpenGL shaders
		RM_EMBREE, // Use Embree for CPU ray tracing
		RM_OPTIX,  // Use OptiX for GPU ray tracing
		RM_HYBRID, // Use both Embree and OptiX for CPU and GPU ray tracing
	};

	RenderMode renderMode;
	bool enableReflections, enableRefractions, enableAo;
	int maxReflections, maxRefractions;
	int aoSamples, aoSamplesSqrt;
	float aoPower, aoNoiseScale;
	void aoInit();
	Image* aoNoiseImage;

	struct Setting {

		Setting(string name, void* variable, bool isBool, float delta, float mi, float ma, float def, function<void()> func);
		void addOption(string name, bool selected, function<void()> func);

		struct Option {
			Option(string name, function<void()> func);
			string name;
			function<void()> func;
		};

		vector<Option> options;
		int selectedOption;

		string name;
		void* variable;
		bool isBool, visible;
		float delta, mi, ma, def;
		function<void()> func;

	};

	Setting* settingScene;
	Setting* settingRenderMode;
	Setting* settingEnableReflections;
	Setting* settingMaxReflections;
	Setting* settingEnableRefractions;
	Setting* settingMaxRefractions;
	Setting* settingEnableAo;
	Setting* settingAoSamples;
	Setting* settingAoRadius;
	Setting* settingAoPower;
	Setting* settingAoNoiseScale;
	Setting* settingEmbreeRenderTiles;
	Setting* settingEmbreePacketPrimary;
	Setting* settingEmbreePacketSecondary;
	Setting* settingEmbreeTileWidth;
	Setting* settingEmbreeTileHeight;
	Setting* settingOptixStackSize;
	Setting* settingOptixBuilder;
	Setting* settingOptixTraverser;
	Setting* settingHybridBalanceMode;
	Setting* settingHybridPartition;
	Setting* settingHybridDisplayPartition;
	vector<Setting*> settings;
	int selectedSetting;

	void settingsInit();
	void settingsInput();
	void settingsUpdate();
	Setting* addSetting(string name, function<void()> func = nullptr);
	Setting* addSettingVariable(string name, void* variable, float delta, float mi, float ma, float def, function<void()> func = nullptr);
	Setting* addSettingVariableBool(string name, void* variable, bool def, function<void()> func = nullptr);

	//// GUI ////

	Font* fntGui;
	Font* fntGuiBold;
	int guiHeight;

	bool showGui;
	void guiRender();
	void guiRenderSetting(Setting* setting, int x, int& y);
	void guiRenderText(string text, int x, int& y, Color color);
	void guiRenderTextBold(string text, int x, int& y, Color color);

	//// OpenGL ////

	struct OpenGL {

		Shader* shdrColor;
		Shader* shdrTexture;
		Shader* shdrNormals;
		Shader* shdrPhong;

	} OpenGL;

	void openglRender();
	void openglSetupNormals(GLuint program, Object* object, TriangleMesh* mesh);
	void openglSetupPhong(GLuint program, Object* object, TriangleMesh* mesh);

	//// Embree ///

	struct Embree {

		struct Ray : RTCRay {
			int x, y;
		};

		struct LightRay : Ray {
			float attenuation;
		};

		struct RayPacket : EMBREE_PACKET_TYPE {
			int valid[EMBREE_PACKET_SIZE];
			int x, y;
		};

		struct LightRayPacket : RayPacket {
			float attenuation[EMBREE_PACKET_SIZE];
			float distance[EMBREE_PACKET_SIZE];
			Vec3 incidence[EMBREE_PACKET_SIZE];
		};

		RTCDevice device;
		vector<Color> buffer;
		GLuint texture;
		int offset, width, frames;
		float time, lastTime, avgTime;

		bool renderTiles, packetPrimary, packetSecondary;
		int tileWidth, tileHeight;

	} Embree;

	void embreeInit();
	void embreeResize();
	void embreeRender();
	void embreeRenderFirePrimaryRay(int x, int y);
	void embreeRenderFirePrimaryPacket(int x, int y);
	void embreeRenderTraceRay(Embree::Ray& ray, int reflectDepth, int refractDepth, Color& result);
	void embreeRenderTracePacket(Embree::RayPacket& packet, int reflectDepth, int refractDepth, Color* result);
	void embreeRenderUpdateTexture();
	Color embreeRenderSky(Vec3 dir);
	static void embreeOcclusionFilter(void* data, Embree::LightRay& ray);
	static void embreeOcclusionFilter8(int* valid, void* data, Embree::LightRayPacket& packet);

	//// OptiX ////

	struct Optix  {

		optix::Context context;
		optix::Buffer renderBuffer, lights;
		optix::TextureSampler aoNoise;
		GLuint vbo;
		GLuint texture;
		int offset, width, frames;
		float time, lastTime, avgTime;

	} Optix;

	void optixInit();
	void optixSetScene(Scene* scene);
	void optixResize();
	void optixRender();
	void optixRenderUpdateTexture();

	//// Hybrid ////

	enum BalanceMode {
		BM_RENDER_TIME,
		BM_MANUAL
	};

	struct Hybrid {

		BalanceMode balanceMode;
		float partition;
		float direction;
		bool displayPartition;

	} Hybrid;

	void hybridInit();
	void hybridRender();
	void hybridRenderUpdatePartition();

};