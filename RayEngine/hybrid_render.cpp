#include "rayengine.h"

void RayEngine::hybridRender() {

	Hybrid.timer.start();

	if (Hybrid.threaded) {

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

	Hybrid.timer.stop();

}
