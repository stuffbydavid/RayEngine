#include "rayengine.h"

void RayEngine::renderHybrid() {

	renderOptix();
	renderEmbree();

	/*omp_set_nested(1);
	//omp_set_num_threads(4);

    #pragma omp parallel 
	{

        #pragma omp master
		renderOptix();

        #pragma omp single
		renderEmbree();

        //#pragma omp barrier //??
	}*/

}