#include "rayengine.h"
#include "lodepng.h"

void RayEngine::saveRender() {

	int pw, ph, pr;
	Color* buf;
	pw = window.width;
	ph = window.height;
	pr = window.ratio;
	window.width = 1920;
	window.height = 1080;
	window.ratio = 1920.f / 1080.f;

	rayXaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
	rayYaxis = -curCamera->yaxis * curCamera->tFov;
	rayZaxis = curCamera->zaxis;

	if (renderMode == RM_EMBREE) {
		embreeResize();
		embreeUpdatePartition();
		embreeRender();
		buf = &Embree.buffer[0];
	} else if (renderMode == RM_OPTIX) {
		optixResize();
		optixUpdatePartition();
		optixRender();
		buf = (Color*)Optix.renderBuffer->map();
	}

	vector<uchar> pngData(window.width * window.height * 4);
	for (uint i = 0; i < window.width * window.height; i++) {
		pngData[i * 4 + 0] = clamp(buf[i].r(), 0, 1) * 255;
		pngData[i * 4 + 1] = clamp(buf[i].g(), 0, 1) * 255;
		pngData[i * 4 + 2] = clamp(buf[i].b(), 0, 1) * 255;
		pngData[i * 4 + 3] = 255;
	}
	lodepng::encode("renders/" + date() + ".png", pngData, window.width, window.height);

	window.width = pw;
	window.height = ph;
	window.ratio = pr;

	if (renderMode == RM_EMBREE) {
		embreeResize();
		embreeUpdatePartition();
	} else if (renderMode == RM_OPTIX) {
		Optix.renderBuffer->unmap();
		optixResize();
		optixUpdatePartition();
	}

}