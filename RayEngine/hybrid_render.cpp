#include "rayengine.h"
#include <omp.h>

void RayEngine::hybridRender() {

	Hybrid.renderTimer.start();

	if (Hybrid.enableThreaded) {

		omp_set_nested(true);
		#pragma omp parallel num_threads(2)
		{
			if (omp_get_thread_num() == 0)
				optixRender();
			else
				embreeRender();
		}
		embreeRenderUpdateTexture();
		optixRenderUpdateTexture();

	} else {

		optixRender();
		embreeRender();
		embreeRenderUpdateTexture();
		optixRenderUpdateTexture();

	}

	Hybrid.renderTimer.stop();

}
