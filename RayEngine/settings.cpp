#include "rayengine.h"
#include <omp.h>

void RayEngine::settingsInit() {

	showGui = true;
	guiHeight = 100;
	selectedSetting = 0;

	// Scene
	settingScene = addSetting("Scene", [this]() { optixSetScene(curScene); });

	// Render mode
	settingRenderMode = addSetting("Render mode");
	settingRenderMode->addOption("OpenGL", RENDER_MODE == RM_OPENGL, [this]() { renderMode = RM_OPENGL; });
	settingRenderMode->addOption("Embree", RENDER_MODE == RM_EMBREE, [this]() { renderMode = RM_EMBREE; resize(); });
	settingRenderMode->addOption("OptiX",  RENDER_MODE == RM_OPTIX,  [this]() { renderMode = RM_OPTIX;  resize(); });
	settingRenderMode->addOption("Hybrid", RENDER_MODE == RM_HYBRID, [this]() { renderMode = RM_HYBRID; resize(); });

	// Window size
	settingResolution = addSetting("Resolution");
	settingResolution->addOption("800x600",  true,   [this]() { glfwSetWindowSize(window.handle, 800, 600); });
	settingResolution->addOption("1280x720", false,  [this]() { glfwSetWindowSize(window.handle, 1280, 720); });
	settingResolution->addOption("1920x1080", false, [this]() { glfwSetWindowSize(window.handle, 1920, 1080); });
	
	// Camera path
	settingCameraPath = addSettingVariableBool("Camera path", &enableCameraPath, false);

	// Reflections/Refractions
	settingEnableReflections = addSettingVariableBool("Reflections", &enableReflections, ENABLE_REFLECTIONS);
	settingMaxReflections = addSetting("Max reflections");
	settingEnableRefractions = addSettingVariableBool("Refractions", &enableRefractions, ENABLE_REFRACTIONS);
	settingMaxRefractions = addSetting("Max refractions");
	for (int i = 0; i < 32; i++) {
		settingMaxReflections->addOption(to_string(i), MAX_REFLECTIONS == i, [this, i]() { maxReflections = i; });
		settingMaxRefractions->addOption(to_string(i), MAX_REFRACTIONS == i, [this, i]() { maxRefractions = i; });
	}

	// Ambient occlusion
	settingEnableAo = addSettingVariableBool("Ambient Occlusion", &enableAo, ENABLE_AO);
	settingAoSamples = addSetting("AO samples");
	for (int i = 2; i <= AO_SAMPLES_SQRT_MAX; i++)
		settingAoSamples->addOption(to_string(i * i), AO_SAMPLES_SQRT == i, [this, i]() { aoSamplesSqrt = i; aoSamples = i * i; });
	settingAoRadius = addSettingVariable("AO radius", nullptr, 0.05f, 0.f, 1000.f, 0.f, [this]() { settingAoRadius->delta = *((float*)settingAoRadius->variable) * 0.1f; });
	settingAoPower = addSettingVariable("AO power", &aoPower, 0.025f, 0.f, 10.f, AO_POWER);
	settingAoNoiseScale = addSettingVariable("AO noise scale", &aoNoiseScale, 2.f, 1.f, 100.f, AO_NOISE_SCALE);
	
	// Embree settings
	settingEmbreeNumThreads = addSetting("Embree threads");
	for (int i = 1; i <= 32; i++)
		settingEmbreeNumThreads->addOption(to_string(i), EMBREE_NUM_THREADS == i, [this, i]() { omp_set_num_threads(i); });
	settingEmbreeEnableTiles = addSettingVariableBool("Embree tiles", &Embree.enableTiles, EMBREE_ENABLE_TILES);
	settingEmbreeTileWidth = addSetting("Width");
	settingEmbreeTileHeight = addSetting("Height");
	for (int i = 1; i <= 256; i *= 2) {
		settingEmbreeTileWidth->addOption(to_string(i),  EMBREE_TILE_WIDTH == i,  [this, i]() { Embree.tileWidth = i; });
		settingEmbreeTileHeight->addOption(to_string(i), EMBREE_TILE_HEIGHT == i, [this, i]() { Embree.tileHeight = i; });
	}
	settingEmbreeEnablePacketsPrimary = addSettingVariableBool("Embree primary packets", &Embree.enablePacketsPrimary, EMBREE_ENABLE_PACKETS_PRIMARY);
	settingEmbreeEnablePacketsSecondary = addSettingVariableBool("Secondary packets", &Embree.enablePacketsSecondary, EMBREE_ENABLE_PACKETS_SECONDARY);

	// OptiX settings
	settingOptixEnableProgressive = addSettingVariableBool("OptiX progressive render", &Optix.enableProgressive, OPTIX_ALLOW_PROGRESSIVE && OPTIX_ENABLE_PROGRESSIVE);

	settingOptixStackSize = addSetting("OptiX stack size");
	for (int i = 1024; i <= 65536; i *= 2)
		settingOptixStackSize->addOption(to_string(i), OPTIX_STACK_SIZE == i, [this, i]() { Optix.context->setStackSize(i); });

	// Hybrid settings
	settingHybridEnableThreaded = addSettingVariableBool("Hybrid threaded", &Hybrid.enableThreaded, HYBRID_ENABLE_THREADED);
	settingHybridBalanceMode = addSetting("Hybrid balance mode");
	settingHybridBalanceMode->addOption("Render time",   HYBRID_BALANCE_MODE == BM_RENDER_TIME,   [this]() { Hybrid.balanceMode = BM_RENDER_TIME; });
	settingHybridBalanceMode->addOption("Manual", HYBRID_BALANCE_MODE == BM_MANUAL, [this]() { Hybrid.balanceMode = BM_MANUAL; });
	settingHybridPartition = addSettingVariable("Partition", &Hybrid.partition, 0.01f, 0.f, 1.f, HYBRID_PARTITION, [this]() {resize(); });
	settingHybridDisplayPartition = addSettingVariableBool("Hybrid display partition", &Hybrid.displayPartition, HYBRID_DISPLAY_PARTITION);
	settingHybridEnableEmbree = addSettingVariableBool("Hybrid enable Embree", &Hybrid.enableEmbree, HYBRID_ENABLE_EMBREE);
	settingHybridEnableOptix = addSettingVariableBool("Hybrid enable OptiX", &Hybrid.enableOptix, HYBRID_ENABLE_OPTIX);

	// Add scenes
	for (int i = 0; i < scenes.size(); i++)
		settingScene->addOption(scenes[i]->name, i == 0, [this, i]() { curScene = scenes[i]; curCamera = &curScene->camera; settingAoRadius->variable = &curScene->aoRadius; });

}

RayEngine::Setting::Setting(string name, void* variable, bool isBool, float delta, float mi, float ma, float def, function<void()> func) :
    name(name),
	variable(variable),
	isBool(isBool),
	delta(delta),
	mi(mi),
	ma(ma),
	def(def),
	func(func)
{
	selectedOption = 0;
	visible = false;
}

RayEngine::Setting::Option::Option(string name, function<void()> func) :
    name(name),
    func(func)
{}

RayEngine::Setting* RayEngine::addSetting(string name, function<void()> func) {

	RayEngine::Setting* setting = new RayEngine::Setting(name, nullptr, 1.f, false, 0, 0, 0, func);
	settings.push_back(setting);
	return setting;

}

RayEngine::Setting* RayEngine::addSettingVariable(string name, void* variable, float delta, float mi, float ma, float def, function<void()> func) {

	RayEngine::Setting* setting = new RayEngine::Setting(name, variable, false, delta, mi, ma, def, func);
	settings.push_back(setting);
	if (variable) {
		*((float*)variable) = def;
		if (setting->func)
			setting->func();
	}
	return setting;

}

RayEngine::Setting* RayEngine::addSettingVariableBool(string name, void* variable, bool def, function<void()> func) {

	RayEngine::Setting* setting = new RayEngine::Setting(name, variable, true, 1.f, false, true, def, func);
	settings.push_back(setting);
	*((bool*)variable) = def;
	if (setting->func)
		setting->func();
	return setting;

}

void RayEngine::Setting::addOption(string name, bool selected, function<void()> func) {

	Option option(name, func);
	options.push_back(option);

	if (selected) {
		selectedOption = options.size() - 1;
		func();
	}

}

string RayEngine::Setting::getLogText() {

	if (variable) {
		if (isBool)
			return name + ": " + (*(bool*)variable ? "Yes" : "No");
		else
			return name + ": " + to_string_prec(*(float*)variable, 4);
	}
	return name + ": " + options[selectedOption].name;

}

void RayEngine::settingsInput() {

	// Toggle GUI

	if (window.keyPressed[GLFW_KEY_F1])
		showGui = !showGui;

	// Start/stop benchmarking

	if (window.keyPressed[GLFW_KEY_F2]) {
		benchmarkMode = !benchmarkMode;
		if (benchmarkMode)
			benchmarkStart();
		else
			benchmarkStop();
	}

	// Save screenshot

	if (window.keyPressed[GLFW_KEY_F3])
		saveRender();

	// Print camera

	if (window.keyPressed[GLFW_KEY_F11]) {
		cout << curCamera->position << endl;
		cout << curCamera->xaxis << endl;
		cout << curCamera->yaxis << endl;
		cout << curCamera->zaxis << endl;
		cout << endl;
	}

	// Settings

	if (showGui) {

		Setting* curSetting = settings[selectedSetting];

		// Scroll settings list

		if (window.keyPressed[GLFW_KEY_UP]) {
			do {
				selectedSetting = mod(selectedSetting - 1, settings.size());
				curSetting = settings[selectedSetting];
			} while (!curSetting->visible);
		}

		if (window.keyPressed[GLFW_KEY_DOWN]) {
			do {
				selectedSetting = mod(selectedSetting + 1, settings.size());
				curSetting = settings[selectedSetting];
			} while (!curSetting->visible);
		}

		if (curSetting->variable) { // Increase/decrease variable value

			if (curSetting->isBool) { // Switch between false and true

				if (window.keyPressed[GLFW_KEY_LEFT] || window.keyPressed[GLFW_KEY_RIGHT]) {
					*((bool*)curSetting->variable) = !*((bool*)curSetting->variable);
					if (curSetting->func)
						curSetting->func();
					LOG("Changed " + curSetting->getLogText());
				}

			} else { // Increase/decrease

				if (window.keyDown[GLFW_KEY_LEFT]) {
					*((float*)curSetting->variable) = clamp(*((float*)curSetting->variable) - curSetting->delta, curSetting->mi, curSetting->ma);
					if (curSetting->func)
						curSetting->func();
					LOG("Changed " + curSetting->getLogText());
				}

				if (window.keyDown[GLFW_KEY_RIGHT]) {
					*((float*)curSetting->variable) = clamp(*((float*)curSetting->variable) + curSetting->delta, curSetting->mi, curSetting->ma);
					if (curSetting->func)
						curSetting->func();
					LOG("Changed " + curSetting->getLogText());
				}

			}

		} else { // Scroll options list

			if (window.keyPressed[GLFW_KEY_LEFT]) {

				if (curSetting->options.size() > 2)
					curSetting->selectedOption = clamp(curSetting->selectedOption - 1, 0, curSetting->options.size() - 1);
				else
					curSetting->selectedOption = mod(curSetting->selectedOption - 1, curSetting->options.size());

				curSetting->options[curSetting->selectedOption].func();
				if (curSetting->func)
					curSetting->func();
				LOG("Changed " + curSetting->getLogText());

			}

			if (window.keyPressed[GLFW_KEY_RIGHT]) {

				if (curSetting->options.size() > 2)
					curSetting->selectedOption = clamp(curSetting->selectedOption + 1, 0, curSetting->options.size() - 1);
				else
					curSetting->selectedOption = mod(curSetting->selectedOption + 1, curSetting->options.size());
				curSetting->options[curSetting->selectedOption].func();

				if (curSetting->func)
					curSetting->func();
				LOG("Changed " + curSetting->getLogText());

			}

		}

	}

}