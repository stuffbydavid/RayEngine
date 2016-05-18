#include "rayengine.h"

template <typename t>
string to_string_prec(t val, int prec) {

	stringstream ss;
	ss << setprecision(prec) << val;
	return ss.str();

}

void RayEngine::guiRender() {

	for (Setting* s : settings)
		s->visible = false;

	int dx = 20, dy = 20, indent = 20;

	// Background box
	OpenGL.shdrColor->render2DBox(window.ortho, 10, 10, 250, guiHeight, 0, { 0.f, 0.f, 0.f, 0.75f });

	// Title
	guiRenderTextBold("RayEngine 0.1 Alpha", dx, dy); dy += 30;

	// Fps
	guiRenderText("Fps: " + to_string(window.fps), dx, dy); dy += 16;

	// Screen size
	guiRenderText("Screen: " + to_string(window.width) + "x" + to_string(window.height), dx, dy); dy += 16;

	// Embree average time
	if (renderMode == RM_EMBREE || renderMode == RM_HYBRID) {
		guiRenderText("Embree avg render:", dx, dy);
		guiRenderText(to_string_prec(Embree.renderTimer.avgTime, 4) + " s", dx + 150, dy); dy += 16;
		guiRenderText("Embree avg texture:", dx, dy);
		guiRenderText(to_string_prec(Embree.textureTimer.avgTime, 4) + " s", dx + 150, dy); dy += 16;
	}

	// Optix average time
	if (renderMode == RM_OPTIX || renderMode == RM_HYBRID) {
		guiRenderText("Optix avg render:", dx, dy);
		guiRenderText(to_string_prec(Optix.renderTimer.avgTime, 4) + " s", dx + 150, dy); dy += 16;
		guiRenderText("Optix avg texture:", dx, dy);
		guiRenderText(to_string_prec(Optix.textureTimer.avgTime, 4) + " s", dx + 150, dy); dy += 16;
	}

	// Hybrid average time
	if (renderMode == RM_HYBRID) {
		guiRenderText("Hybrid avg render:", dx, dy);
		guiRenderText(to_string_prec(Hybrid.timer.avgTime, 4) + " s", dx + 150, dy); dy += 16;
	}

	// Settings
	dy += 10;

	// Scene/Render Mode/Camera
	guiRenderSetting(settingScene, dx, dy);
	guiRenderSetting(settingRenderMode, dx, dy);
	if (curScene->cameraPath)
		guiRenderSetting(settingCameraPath, dx, dy);
	dy += 8;

	if (renderMode != RM_OPENGL) {

		// Reflections
		guiRenderSetting(settingEnableReflections, dx, dy);
		if (enableReflections)
			guiRenderSetting(settingMaxReflections, dx + indent, dy);

		// Refractions
		guiRenderSetting(settingEnableRefractions, dx, dy);
		if (enableRefractions)
			guiRenderSetting(settingMaxRefractions, dx + indent, dy);

		// Ambient Occlusion
		guiRenderSetting(settingEnableAo, dx, dy);
		if (enableAo) {
			guiRenderSetting(settingAoSamples, dx + indent, dy);
			guiRenderSetting(settingAoRadius, dx + indent, dy);
			guiRenderSetting(settingAoPower, dx + indent, dy);
			guiRenderSetting(settingAoNoiseScale, dx + indent, dy);
		}
		dy += 8;

		// Embree
		if (renderMode == RM_EMBREE || renderMode == RM_HYBRID) {
			guiRenderSetting(settingEmbreeNumThreads, dx, dy);
			guiRenderSetting(settingEmbreeRenderTiles, dx, dy);
			if (Embree.renderTiles) {
				guiRenderSetting(settingEmbreeTileWidth, dx + indent, dy);
				guiRenderSetting(settingEmbreeTileHeight, dx + indent, dy);
			}
			guiRenderSetting(settingEmbreePacketPrimary, dx, dy);
			if (Embree.packetPrimary)
				guiRenderSetting(settingEmbreePacketSecondary, dx + indent, dy);
			dy += 8;
		}

		// OptiX
		if (renderMode == RM_OPTIX || renderMode == RM_HYBRID) {
			guiRenderSetting(settingOptixProgressive, dx, dy);
			guiRenderSetting(settingOptixStackSize, dx, dy);
			dy += 8;
		}

		// Hybrid
		if (renderMode == RM_HYBRID) {
			guiRenderSetting(settingHybridThreaded, dx, dy);
			guiRenderSetting(settingHybridBalanceMode, dx, dy);
			if (Hybrid.balanceMode == BM_MANUAL)
				guiRenderSetting(settingHybridPartition, dx + indent, dy);
			guiRenderSetting(settingHybridDisplayPartition, dx, dy);
			guiRenderSetting(settingHybridEmbree, dx, dy);
			guiRenderSetting(settingHybridOptix, dx, dy);
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
			valueText = to_string_prec(val, 3);

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