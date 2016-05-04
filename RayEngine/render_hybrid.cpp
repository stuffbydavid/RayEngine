#include "rayengine.h"

void RayEngine::renderHybrid() {

	#if HYBRID_THREADED
		omp_set_nested(true);
		#pragma omp parallel num_threads(2)
		{
			if (omp_get_thread_num() == 0)
				renderOptix();
			else
				renderEmbree();
		}
	#else
		renderEmbree();
		renderOptix();
	#endif

	renderEmbreeTexture();
	renderOptixTexture();
}