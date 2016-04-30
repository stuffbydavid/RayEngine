#include "rayengine.h"

void RayEngine::renderHybrid() {

	omp_set_nested(true);
	
	// Embree slows down in hybrid mode, OptiX statys the same
    #pragma omp parallel num_threads(2)
	{
		if (omp_get_thread_num() == 0) {
			renderOptix();
		} else {
			renderEmbree();
		}
	}

	renderEmbreeTexture();
	renderOptixTexture();
}