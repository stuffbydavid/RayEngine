#pragma once
#pragma warning(disable: 4005) // Macro redefinitions from Embree+OptiX

#include "util.h"
#include "vec3.h"
#include "window.h"
#include "scene.h"
#include "font.h"
#include "settings.h"

#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_math_namespace.h>
#include "ft2build.h"
#include FT_FREETYPE_H

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
	bool enableCameraPath, enableReflections, enableRefractions, enableAo;
	int maxReflections, maxRefractions;
	int aoSamples, aoSamplesSqrt;
	float aoPower, aoNoiseScale;
	void aoInit();
	Image* aoNoiseImage;

	struct Setting {

		Setting(string name, void* variable, bool isBool, float delta, float mi, float ma, float def, function<void()> func);
		void addOption(string name, bool selected, function<void()> func);
		string getLogText();

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
	Setting* settingResolution;
	Setting* settingCameraPath;
	Setting* settingEnableReflections;
	Setting* settingMaxReflections;
	Setting* settingEnableRefractions;
	Setting* settingMaxRefractions;
	Setting* settingEnableAo;
	Setting* settingAoSamples;
	Setting* settingAoRadius;
	Setting* settingAoPower;
	Setting* settingAoNoiseScale;
	Setting* settingEmbreeNumThreads;
	Setting* settingEmbreeEnableTiles;
	Setting* settingEmbreeEnablePacketsPrimary;
	Setting* settingEmbreeEnablePacketsSecondary;
	Setting* settingEmbreeTileWidth;
	Setting* settingEmbreeTileHeight;
	Setting* settingOptixEnableProgressive;
	Setting* settingOptixStackSize;
	Setting* settingHybridEnableThreaded;
	Setting* settingHybridBalanceMode;
	Setting* settingHybridPartition;
	Setting* settingHybridDisplayPartition; 
	Setting* settingHybridEnableEmbree;
	Setting* settingHybridEnableOptix;
	vector<Setting*> settings;
	int selectedSetting;

	void settingsInit();
	void settingsInput();
	void settingsUpdate();
	void saveRender();
	Setting* addSetting(string name, function<void()> func = nullptr);
	Setting* addSettingVariable(string name, void* variable, float delta, float mi, float ma, float def, function<void()> func = nullptr);
	Setting* addSettingVariableBool(string name, void* variable, bool def, function<void()> func = nullptr);

	//// GUI ////

	Font* fntGui;
	Font* fntGuiBold;
	Font* fntGuiBig;
	int guiHeight;

	bool showGui;
	void guiRender();
	void guiRenderSetting(Setting* setting, int& x, int& y, bool indent = false);
	void guiRenderText(string text, int x, int y, Color color = { 1.f });
	void guiRenderTextBold(string text, int x, int y, Color color = { 1.f });
	void guiRenderTextBig(string text, int x, int y, Color color = { 1.f });

	//// Benchmarking ////

	void logInit();
	void logClose();
	string logFileName;
	ofstream logFileStream;

	bool benchmarkMode;
	void benchmarkUpdate();
	void benchmarkStart();
	void benchmarkStop();

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

	//// Timer ////

	struct Timer {

		Timer();

		void start();
		void stop();

		int avgAdditions;
		float avgTime, lastTime;
		float startMeasure, avgTotal, lastAvgMeasure;

	};

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
		int offset, width;
		bool enableTiles, enablePacketsPrimary, enablePacketsSecondary;
		int tileWidth, tileHeight, numThreads;

		Timer renderTimer, textureTimer;

	} Embree;

	void embreeInit();
	void embreeUpdatePartition();
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
		optix::Buffer renderBuffer, streamRenderBuffer, streamBuffer, lights;
		optix::TextureSampler aoNoise;
		GLuint vbo;
		GLuint texture;
		bool enableProgressive;
		int offset, width;
		Timer renderTimer, textureTimer;

	} Optix;

	void optixInit();
	void optixSetScene(Scene* scene);
	void optixUpdatePartition();
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
		bool enableThreaded, displayPartition, enableEmbree, enableOptix;
		Timer renderTimer;

	} Hybrid;

	void hybridInit();
	void hybridRender();
	void hybridRenderUpdatePartition();

};