#include "rayengine.h"

void RayEngine::guiRender() {

	OpenGL.shdrColor->render2DBox(window.ortho, 10, 10, 250, 200, 0, { 0.f, 0.f, 0.f, 0.5f });

	int dx = 20, dy = 20;
	fntGuiBold->renderText("RayEngine 0.1 Alpha", dx, dy, { 1.f }, OpenGL.shdrTexture, window.ortho);
	dy += 30;

	guiRenderText("Fps: " + to_string(window.fps), dx, dy, { 1.f });
	dy += 30;

	guiRenderSetting(settingRenderMode, dx, dy);
	dy += 16;
	guiRenderSetting(settingMaxReflections, dx, dy);
	dy += 16;

	guiRenderSetting(settingEmbreeRenderTiles, dx, dy);
	dy += 16;
	guiRenderSetting(settingEmbreeTileWidth, dx, dy);
	dy += 16;
	guiRenderSetting(settingEmbreeTileHeight, dx, dy);
	dy += 16;
	guiRenderSetting(settingEmbreePacketPrimary, dx, dy);
	dy += 16;
	guiRenderSetting(settingEmbreePacketSecondary, dx, dy);
	dy += 16;

}

void RayEngine::guiRenderText(string text, int x, int y, Color color) {

	fntGui->renderText(text, x, y, color, OpenGL.shdrTexture, window.ortho);
}

void RayEngine::guiRenderSetting(Setting* setting, int x, int y) {

	Color color = { 1.f };
	if (settings[selectedSetting] == setting)
		color = { 1.f, 1.f, 0.f };

	guiRenderText(setting->name, x, y, color);
	guiRenderText(setting->options[setting->selectedOption].name, x + 150, y, color);

}