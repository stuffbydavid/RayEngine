#include "rayengine.h"

void RayEngine::renderHybrid() {

	omp_set_nested(true);
	
#if HYBRID_THREADED
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