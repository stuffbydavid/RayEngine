#pragma once

#include "common.h"
#include "settings.h"
#include "window.h"
#include "scene.h"
#include "shader.h"
#include "font.h"

struct RayEngine {

	//todo: public vs private

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
	Scene* curScene;
	Camera* curCamera;
	Vec3 rayOrg, rayXaxis, rayYaxis, rayZaxis;

	Scene* createScene(string name, Color ambient = { 0.f }, string skyFile = "", Color skyColor = { 0.f });
	void cameraInput();

	//// Settings ////

	enum RenderMode {
		RM_OPENGL, // Render using OpenGL shaders
		RM_EMBREE, // Use Embree for CPU ray tracing
		RM_OPTIX,  // Use OptiX for GPU ray tracing
		RM_HYBRID, // Use both Embree and OptiX for CPU and GPU ray tracing
	};

	RenderMode renderMode;
	int maxReflections, maxRefractions;

	struct Setting {

		Setting(string name);
		void addOption(string name, function<void()> func, bool selected = false);

		struct Option {
			Option(string name, function<void()> func);
			string name;
			function<void()> func;
		};

		vector<Option> options;
		string name;
		int selectedOption;

	};

	Setting* settingRenderMode;
	Setting* settingMaxReflections;
	Setting* settingMaxRefractions;
	Setting* settingEmbreeRenderTiles;
	Setting* settingEmbreePacketPrimary;
	Setting* settingEmbreePacketSecondary;
	Setting* settingEmbreeTileWidth;
	Setting* settingEmbreeTileHeight;
	Setting* settingOptixStackSize;
	Setting* settingHybridBalanceMode;
	Setting* settingHybridDisplayPartition;
	vector<Setting*> settings;
	int selectedSetting;

	void settingsInit();
	void settingsInput();
	void settingsUpdate();
	Setting* addSetting(string name);

	//// GUI ////

	Font* fntGui;
	Font* fntGuiBold;

	bool showGui;
	void guiRender();
	void guiRenderSetting(Setting* setting, int x, int& y, bool indent = false);
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
			Color transColor;
		};
		struct RayPacket : EMBREE_PACKET_TYPE {
			Color transColor[EMBREE_PACKET_SIZE];
			int valid[EMBREE_PACKET_SIZE];
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
	void embreeRenderTraceRay(Embree::Ray& ray, int depth, Color& result);
	void embreeRenderTracePacket(Embree::RayPacket& packet, int depth, Color* result);
	void embreeRenderUpdateTexture();
	Color embreeRenderSky(Vec3 dir);

	//// OptiX ////

	struct Optix  {

		optix::Context context;
		optix::Buffer renderBuffer, lights;
		GLuint vbo;
		GLuint texture;
		int offset, width, frames;
		float time, lastTime, avgTime;

	} Optix;

	void optixInit();
	void optixResize();
	void optixRender();
	void optixRenderUpdateTexture();

	//// Hybrid ////

	enum BalanceMode {
		BM_TIME,
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