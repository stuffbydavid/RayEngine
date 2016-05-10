#include "rayengine.h"

void RayEngine::hybridRender() {

	// TODO: Look into TBB
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
		optixRender();
		embreeRender();
	#endif

	embreeRenderUpdateTexture();
	optixRenderUpdateTexture();
}
