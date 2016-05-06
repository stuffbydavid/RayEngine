#include "rayengine.h"

void RayEngine::hybridRender() {

	#if HYBRID_THREADED
		omp_set_nested(true);
		#pragma omp parallel num_threads(2)
		{
			if (omp_get_thread_num() == 0)
				optixRender();
			else
				embreeRender();
		}
	#else
		embreeRender();
		optixRender();
	#endif

	embreeRenderUpdateTexture();
	optixRenderUpdateTexture();
}