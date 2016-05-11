#include "rayengine.h"

void RayEngine::settingsInit() {

	selectedSetting = 0;

	//// Render mode ////

	settingRenderMode = addSetting("Render mode");
	settingRenderMode->addOption("OpenGL", [this]() { renderMode = RM_OPENGL; }, RENDER_MODE == RM_OPENGL);
	settingRenderMode->addOption("Embree", [this]() { renderMode = RM_EMBREE; resize(); }, RENDER_MODE == RM_EMBREE);
	settingRenderMode->addOption("OptiX", [this]() { renderMode = RM_OPTIX;  resize(); }, RENDER_MODE == RM_OPTIX);
	settingRenderMode->addOption("Hybrid", [this]() { renderMode = RM_HYBRID; resize(); }, RENDER_MODE == RM_HYBRID);

	//// Max reflections ////

	settingMaxReflections = addSetting("Max reflections");
	settingMaxRefractions = addSetting("Max refractions");
	for (int i = 0; i < 32; i++) {
		settingMaxReflections->addOption(to_string(i), [this, i]() { maxReflections = i; }, MAX_REFLECTIONS == i);
		settingMaxRefractions->addOption(to_string(i), [this, i]() { maxRefractions = i; }, MAX_REFRACTIONS == i);
	}

	//// Embree settings ////
	settingEmbreeRenderTiles = addSetting("Render tiles");
	settingEmbreeRenderTiles->addOption("Yes", [this]() { Embree.renderTiles = true; },   EMBREE_RENDER_TILES);
	settingEmbreeRenderTiles->addOption("No",  [this]() { Embree.renderTiles = false; }, !EMBREE_RENDER_TILES);

	settingEmbreeTileWidth = addSetting("Width");
	settingEmbreeTileHeight = addSetting("Height");
	for (int i = 1; i <= 256; i *= 2) {
		settingEmbreeTileWidth->addOption(to_string(i),  [this, i]() { Embree.tileWidth = i; },  EMBREE_TILE_WIDTH == i);
		settingEmbreeTileHeight->addOption(to_string(i), [this, i]() { Embree.tileHeight = i; }, EMBREE_TILE_HEIGHT == i);
	}

	settingEmbreePacketPrimary = addSetting("Primary packets");
	settingEmbreePacketPrimary->addOption("Yes", [this]() { Embree.packetPrimary = true; },   EMBREE_PACKET_PRIMARY);
	settingEmbreePacketPrimary->addOption("No",  [this]() { Embree.packetPrimary = false; }, !EMBREE_PACKET_PRIMARY);

	settingEmbreePacketSecondary = addSetting("Secondary packets");
	settingEmbreePacketSecondary->addOption("Yes", [this]() { Embree.packetSecondary = true; },   EMBREE_PACKET_SECONDARY);
	settingEmbreePacketSecondary->addOption("No", [this]() { Embree.packetSecondary = false; }, !EMBREE_PACKET_SECONDARY);

	//// OptiX settings ////

	settingOptixStackSize = addSetting("Stack size");
	for (int i = 1024; i <= 65536; i *= 2)
		settingOptixStackSize->addOption(to_string(i), [this, i]() { Optix.context->setStackSize(i); }, OPTIX_STACK_SIZE == i);

	//// Hybrid settings ////

	settingHybridBalanceMode = addSetting("Balance mode");
	settingHybridBalanceMode->addOption("Time",   [this]() { Hybrid.balanceMode = BM_TIME; },   HYBRID_BALANCE_MODE == BM_TIME);
	settingHybridBalanceMode->addOption("Manual", [this]() { Hybrid.balanceMode = BM_MANUAL; }, HYBRID_BALANCE_MODE == BM_MANUAL);

	settingHybridDisplayPartition = addSetting("Display partition");
	settingHybridDisplayPartition->addOption("Yes", [this]() { Hybrid.displayPartition = true; },   HYBRID_DISPLAY_PARTITION);
	settingHybridDisplayPartition->addOption("No",  [this]() { Hybrid.displayPartition = false; }, !HYBRID_DISPLAY_PARTITION);

}

RayEngine::Setting::Setting(string name) :
    name(name)
{
	selectedOption = 0;
}

RayEngine::Setting::Option::Option(string name, function<void()> func) :
    name(name),
    func(func)
{}

RayEngine::Setting* RayEngine::addSetting(string name) {

	RayEngine::Setting* setting = new RayEngine::Setting(name);
	settings.push_back(setting);
	return setting;

}

void RayEngine::Setting::addOption(string name, function<void()> func, bool selected) {

	Option option(name, func);
	options.push_back(option);

	if (selected) {
		selectedOption = options.size() - 1;
		func();
	}

}

void RayEngine::settingsInput() {

	// Toggle GUI

	if (window.keyPressed[GLFW_KEY_F1])
		showGui = !showGui;

	// Print camera

	if (window.keyPressed[GLFW_KEY_F11]) {
		cout << curCamera->position << endl;
		cout << curCamera->xaxis << endl;
		cout << curCamera->yaxis << endl;
		cout << curCamera->zaxis << endl;
		cout << endl;
	}

	// Hybrid partition

	/*if (window.keyDown[GLFW_KEY_LEFT]) {
		hybridPartition = max(hybridPartition - 0.01f, 0.f);
		resize();
	}

	if (window.keyDown[GLFW_KEY_RIGHT]) {
		hybridPartition = min(hybridPartition + 0.01f, 1.f);
		resize();
	}*/

	// Settings

	if (window.keyPressed[GLFW_KEY_UP])
		selectedSetting = mod(selectedSetting - 1, settings.size());

	if (window.keyPressed[GLFW_KEY_DOWN])
		selectedSetting = mod(selectedSetting + 1, settings.size());

	if (window.keyPressed[GLFW_KEY_LEFT]) {
		Setting* curSetting = settings[selectedSetting];
		if (curSetting->options.size() > 2)
			curSetting->selectedOption = clamp(curSetting->selectedOption - 1, 0, curSetting->options.size() - 1);
		else
			curSetting->selectedOption = mod(curSetting->selectedOption - 1, curSetting->options.size());
		curSetting->options[curSetting->selectedOption].func();
	}

	if (window.keyPressed[GLFW_KEY_RIGHT]) {
		Setting* curSetting = settings[selectedSetting];
		if (curSetting->options.size() > 2)
			curSetting->selectedOption = clamp(curSetting->selectedOption + 1, 0, curSetting->options.size() - 1);
		else
			curSetting->selectedOption = mod(curSetting->selectedOption + 1, curSetting->options.size());
		curSetting->options[curSetting->selectedOption].func();
	}

}