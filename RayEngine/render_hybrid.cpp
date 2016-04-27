#include "rayengine.h"

void RayEngine::renderHybrid() {

	omp_set_nested(1);

    #pragma omp parallel num_threads(2)
	{
		if (omp_get_thread_num() == 0)
			renderOptix();
		else {
			renderEmbree();
		}
	}

	renderEmbreeTexture();
	renderOptixTexture();

}