#include "rayengine.h"

void RayEngine::guiRender() {

	if (showGui) {
		for (Setting* s : settings)
			s->visible = false;

		int dx = 20, dy = 20;

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
			//guiRenderText("Embree avg texture:", dx, dy);
			//guiRenderText(to_string_prec(Embree.textureTimer.avgTime, 4) + " s", dx + 150, dy); dy += 16;
		}

		// Optix average time
		if (renderMode == RM_OPTIX || renderMode == RM_HYBRID) {
			guiRenderText("Optix avg render:", dx, dy);
			guiRenderText(to_string_prec(Optix.renderTimer.avgTime, 4) + " s", dx + 150, dy); dy += 16;
			//guiRenderText("Optix avg texture:", dx, dy);
			//guiRenderText(to_string_prec(Optix.textureTimer.avgTime, 4) + " s", dx + 150, dy); dy += 16;
		}

		// Hybrid average time
		if (renderMode == RM_HYBRID) {
			guiRenderText("Hybrid avg render:", dx, dy);
			guiRenderText(to_string_prec(Hybrid.renderTimer.avgTime, 4) + " s", dx + 150, dy); dy += 16;
		}

		// Settings
		dy += 10;

		// Scene
		guiRenderSetting(settingScene, dx, dy);

		// Render mode
		guiRenderSetting(settingRenderMode, dx, dy);

		// Resolution
		guiRenderSetting(settingResolution, dx, dy);

		// Camera
		if (curScene->cameraPath)
			guiRenderSetting(settingCameraPath, dx, dy);
		dy += 8;

		if (renderMode != RM_OPENGL) {

			// Reflections
			guiRenderSetting(settingEnableReflections, dx, dy);
			if (enableReflections)
				guiRenderSetting(settingMaxReflections, dx, dy, true);

			// Refractions
			guiRenderSetting(settingEnableRefractions, dx, dy);
			if (enableRefractions)
				guiRenderSetting(settingMaxRefractions, dx, dy, true);

			// Ambient Occlusion
			guiRenderSetting(settingEnableAo, dx, dy);
			if (enableAo) {
				guiRenderSetting(settingAoSamples, dx, dy, true);
				guiRenderSetting(settingAoRadius, dx, dy, true);
				guiRenderSetting(settingAoPower, dx, dy, true);
				guiRenderSetting(settingAoNoiseScale, dx, dy, true);
			}
			dy += 8;

			// Embree
			if (renderMode == RM_EMBREE || renderMode == RM_HYBRID) {
				guiRenderSetting(settingEmbreeNumThreads, dx, dy);
				guiRenderSetting(settingEmbreeEnableTiles, dx, dy);
				if (Embree.enableTiles) {
					guiRenderSetting(settingEmbreeTileWidth, dx, dy, true);
					guiRenderSetting(settingEmbreeTileHeight, dx, dy, true);
				}
				guiRenderSetting(settingEmbreeEnablePacketsPrimary, dx, dy);
				if (Embree.enablePacketsPrimary)
					guiRenderSetting(settingEmbreeEnablePacketsSecondary, dx, dy, true);
				dy += 8;
			}

			// OptiX
			if (renderMode == RM_OPTIX || renderMode == RM_HYBRID) {
#if OPTIX_ALLOW_PROGRESSIVE
				guiRenderSetting(settingOptixEnableProgressive, dx, dy);
#endif
				guiRenderSetting(settingOptixStackSize, dx, dy);
				dy += 8;
			}

			// Hybrid
			if (renderMode == RM_HYBRID) {
				guiRenderSetting(settingHybridEnableThreaded, dx, dy);
				guiRenderSetting(settingHybridBalanceMode, dx, dy);
				if (Hybrid.balanceMode == BM_MANUAL)
					guiRenderSetting(settingHybridPartition, dx, dy, true);
				guiRenderSetting(settingHybridDisplayPartition, dx, dy);
				guiRenderSetting(settingHybridEnableEmbree, dx, dy);
				guiRenderSetting(settingHybridEnableOptix, dx, dy);
			}

		}

		guiHeight = dy;

		// Benchmarking
		if (benchmarkMode) {
			dx = window.width - 250;
			dy = window.height - 50;
			guiRenderTextBig("Benchmarking...", dx, dy);
		}

	} else {

		//Fps
		guiRenderTextBig(to_string(window.fps), 8, 8);

	}

}

void RayEngine::guiRenderSetting(Setting* setting, int& x, int& y, bool indent) {

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

	guiRenderText(setting->name, x + 20 * indent, y, color);

	if (selected && !atFirst)
		guiRenderText("<", x + 150 - 12, y, color);
	guiRenderText(valueText + (selected && !atLast ? " > " : ""), x + 150, y, color);

	setting->visible = true;
	y += 16;

}

void RayEngine::guiRenderText(string text, int x, int y, Color color) {

	fntGui->renderText(OpenGL.shdrTexture, window.ortho, text, x + 1, y + 1, { 0.1f });
	fntGui->renderText(OpenGL.shdrTexture, window.ortho, text, x, y, color);

}

void RayEngine::guiRenderTextBold(string text, int x, int y, Color color) {

	fntGuiBold->renderText(OpenGL.shdrTexture, window.ortho, text, x + 1, y + 1, { 0.1f });
	fntGuiBold->renderText(OpenGL.shdrTexture, window.ortho, text, x, y, color);

}

void RayEngine::guiRenderTextBig(string text, int x, int y, Color color) {

	fntGuiBig->renderText(OpenGL.shdrTexture, window.ortho, text, x + 2, y + 2, { 0.1f });
	fntGuiBig->renderText(OpenGL.shdrTexture, window.ortho, text, x, y, color);

}