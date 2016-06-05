#include "rayengine.h"

RayEngine::RayEngine() {

	window.init(WINDOW_WIDTH, WINDOW_HEIGHT);

	// ImageMagick
	Magick::InitializeMagick(NULL);

	// Font
	FT_Library freeType;
	FT_Init_FreeType(&freeType);
	fntGui = new Font(&freeType, "font/tahoma.ttf", 32, 128, 12);
	fntGuiBold = new Font(&freeType, "font/tahomabd.ttf", 32, 128, 12);
	fntGuiBig = new Font(&freeType, "font/tahoma.ttf", 32, 128, 32);

	// Shaders
	OpenGL.shdrColor = new Shader("Color", nullptr, "texture.vshader", "texture.fshader");
	OpenGL.shdrTexture = new Shader("Texture", nullptr, "texture.vshader", "texture.fshader");
	OpenGL.shdrNormals = new Shader("Normals", bind(&RayEngine::openglSetupNormals, this, _1, _2, _3), "normals.vshader", "normals.fshader");
	OpenGL.shdrPhong = new Shader("Phong", bind(&RayEngine::openglSetupPhong, this, _1, _2, _3), "phong.vshader", "phong.fshader");

	benchmarkMode = false;

}

RayEngine::~RayEngine() {
	
	logClose();

}

void RayEngine::launch() {

	for (Scene* s : scenes) {
		int t = 0;
		for (Object* o : s->objects)
			for (Geometry* g : o->geometries)
				t += ((TriangleMesh*)g)->indexData.size();
		cout << s->name << ": " << t << endl;
	}

	aoInit();
	embreeInit();
	optixInit();
	hybridInit();
	settingsInit();
	logInit();

	window.open(bind(&RayEngine::loop, this), bind(&RayEngine::resize, this));

}

void RayEngine::loop() {

	cameraInput();
	settingsInput();
	if (enableCameraPath)
		curScene->updateCameraPath(benchmarkMode);
	
	rayOrg = curCamera->position;
	rayXaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
	rayYaxis = -curCamera->yaxis * curCamera->tFov;
	rayZaxis = curCamera->zaxis;

	if (renderMode == RM_OPENGL) {

		openglRender();

	} else if (renderMode == RM_EMBREE) {

		embreeRender();
		embreeRenderUpdateTexture();

	} else if (renderMode == RM_OPTIX) {

		optixRender();
		optixRenderUpdateTexture();

	} else if (renderMode == RM_HYBRID) {

		hybridRender();
		hybridRenderUpdatePartition();

	}

	if (benchmarkMode)
		benchmarkUpdate();

	guiRender();

	window.setTitle("RayEngine" + (!showGui ? " - FPS: " + to_string(window.fps) : ""));

}

void RayEngine::resize() {

	embreeResize();
	embreeUpdatePartition();
	optixResize();
	optixUpdatePartition();

}

Scene* RayEngine::createScene(string name, string skyFile, Color ambient, float aoRadius, Color background) {

	cout << "Creating scene " << name << "..." << endl;
	Scene* scene = new Scene(name, skyFile, ambient, aoRadius, background);
	scenes.push_back(scene);
	return scene;

}

void RayEngine::aoInit() {

	// Create noise
	Color* noise = new Color[AO_NOISE_WIDTH * AO_NOISE_HEIGHT];
	for (int i = 0; i < AO_NOISE_WIDTH * AO_NOISE_HEIGHT; i++)
		noise[i] = { frand(), frand(), frand() };
	aoNoiseImage = new Image(noise, AO_NOISE_WIDTH, AO_NOISE_HEIGHT, GL_LINEAR);

}