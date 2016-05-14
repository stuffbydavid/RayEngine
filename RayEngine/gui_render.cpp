#include "rayengine.h"

void RayEngine::guiRender() {

	OpenGL.shdrColor->render2DBox(window.ortho, 10, 10, 250, 300, 0, { 0.f, 0.f, 0.f, 0.75f });

	int dx = 20, dy = 20;
	guiRenderTextBold("RayEngine 0.1 Alpha", dx, dy, { 1.f }); dy += 30;

	guiRenderText("Fps: " + to_string(window.fps), dx, dy, { 1.f }); dy += 16;
	guiRenderText("Screen: " + to_string(window.width) + "x" + to_string(window.height), dx, dy, { 1.f }); dy += 30;

	guiRenderSetting(settingScene, dx, dy);
	guiRenderSetting(settingRenderMode, dx, dy);
	guiRenderSetting(settingMaxReflections, dx, dy);
	guiRenderSetting(settingMaxRefractions, dx, dy);

	guiRenderSetting(settingEmbreeRenderTiles, dx, dy);
	guiRenderSetting(settingEmbreeTileWidth, dx, dy, true);
	guiRenderSetting(settingEmbreeTileHeight, dx, dy, true);
	guiRenderSetting(settingEmbreePacketPrimary, dx, dy);
	guiRenderSetting(settingEmbreePacketSecondary, dx, dy);

	guiRenderSetting(settingOptixStackSize, dx, dy);

	guiRenderSetting(settingHybridBalanceMode, dx, dy);
	guiRenderSetting(settingHybridDisplayPartition, dx, dy);

}

void RayEngine::guiRenderSetting(Setting* setting, int x, int& y, bool indent) {

	Color color;
	bool selected = (settings[selectedSetting] == setting);
	bool atFirst = (setting->options.size() > 2 && setting->selectedOption == 0);
	bool atLast = (setting->options.size() > 2 && setting->selectedOption == setting->options.size() - 1);

	if (selected) {
		color = { 1.f, 1.f, 0.f };
		if (!atFirst)
			guiRenderText("<", x + 150 - 12, y, color);
	} else
		color = { 0.5f };

	guiRenderText(setting->name, x + indent * 16, y, color);
	guiRenderText(setting->options[setting->selectedOption].name + (selected && !atLast ? " > " : ""), x + 150, y, color);

	y += 16;

}

void RayEngine::guiRenderText(string text, int x, int& y, Color color) {

	fntGui->renderText(text, x + 1, y + 1, { 0.1f }, OpenGL.shdrTexture, window.ortho);
	fntGui->renderText(text, x, y, color, OpenGL.shdrTexture, window.ortho);

}

void RayEngine::guiRenderTextBold(string text, int x, int& y, Color color) {

	fntGuiBold->renderText(text, x + 1, y + 1, { 0.1f }, OpenGL.shdrTexture, window.ortho);
	fntGuiBold->renderText(text, x, y, color, OpenGL.shdrTexture, window.ortho);

}