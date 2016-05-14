#include "rayengine.h"

void RayEngine::guiRender() {

	for (Setting* s : settings)
		s->visible = false;

	int dx = 20, dy = 20, indent = 20;

	OpenGL.shdrColor->render2DBox(window.ortho, 10, 10, 250, guiHeight, 0, { 0.f, 0.f, 0.f, 0.75f });

	guiRenderTextBold("RayEngine 0.1 Alpha", dx, dy, { 1.f }); dy += 30;

	guiRenderText("Fps: " + to_string(window.fps), dx, dy, { 1.f }); dy += 16;
	guiRenderText("Screen: " + to_string(window.width) + "x" + to_string(window.height), dx, dy, { 1.f }); dy += 30;

	guiRenderSetting(settingScene, dx, dy);
	guiRenderSetting(settingRenderMode, dx, dy);

	if (renderMode != RM_OPENGL) {

		guiRenderSetting(settingEnableReflections, dx, dy);
		if (enableReflections)
			guiRenderSetting(settingMaxReflections, dx + indent, dy);

		guiRenderSetting(settingEnableRefractions, dx, dy);
		if (enableRefractions)
			guiRenderSetting(settingMaxRefractions, dx + indent, dy);

		guiRenderSetting(settingEnableAo, dx, dy);
		if (enableAo) {
			guiRenderSetting(settingAoSamples, dx + indent, dy);
			guiRenderSetting(settingAoRadius, dx + indent, dy);
			guiRenderSetting(settingAoPower, dx + indent, dy);
			guiRenderSetting(settingAoNoiseScale, dx + indent, dy);
		}

		if (renderMode == RM_EMBREE) {
			guiRenderSetting(settingEmbreeRenderTiles, dx, dy);
			if (Embree.renderTiles) {
				guiRenderSetting(settingEmbreeTileWidth, dx + indent, dy);
				guiRenderSetting(settingEmbreeTileHeight, dx + indent, dy);
			}
			guiRenderSetting(settingEmbreePacketPrimary, dx, dy);
			if (Embree.packetPrimary)
				guiRenderSetting(settingEmbreePacketSecondary, dx + indent, dy);
		}

		if (renderMode == RM_OPTIX)
			guiRenderSetting(settingOptixStackSize, dx, dy);

		if (renderMode == RM_HYBRID) {
			guiRenderSetting(settingHybridBalanceMode, dx, dy);
			if (Hybrid.balanceMode == BM_MANUAL)
				guiRenderSetting(settingHybridPartition, dx + indent, dy);

			guiRenderSetting(settingHybridDisplayPartition, dx, dy);
		}

	}

	guiHeight = dy;

}

void RayEngine::guiRenderSetting(Setting* setting, int x, int& y) {

	bool selected = (settings[selectedSetting] == setting);
	bool atFirst, atLast;
	Color color = selected ? Color(1.f, 1.f, 0.f) : Color(0.5f);
	string valueText;

	if (setting->variable) {

		if (setting->isBool) {

			bool val = *((bool*)setting->variable);
			atFirst = atLast = false;
			valueText = val ? "Yes" : "No";

		} else {

			float val = *((float*)setting->variable);
			atFirst = (val == setting->mi);
			atLast = (val == setting->ma);

			stringstream ss;
			ss << setprecision(3) << val;
			valueText = ss.str();

		}

	} else {

		atFirst = (setting->options.size() > 2 && setting->selectedOption == 0);
		atLast = (setting->options.size() > 2 && setting->selectedOption == setting->options.size() - 1);
		valueText = setting->options[setting->selectedOption].name;

	}

	guiRenderText(setting->name, x, y, color);
	if (selected && !atFirst)
		guiRenderText("<", x + 150 - 12, y, color);
	guiRenderText(valueText + (selected && !atLast ? " > " : ""), x + 150, y, color);

	setting->visible = true;
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