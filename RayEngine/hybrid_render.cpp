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

void RayEngine::hybridUpdatePartition() {

	/*if (OptixData.avgTime == 0.f && EmbreeData.avgTime == 0.f)
		return;

	static float lastFrameTime = 0.f;
	float frameTime = OptixData.avgTime + EmbreeData.avgTime;
	float dif = lastFrameTime - frameTime;

	if (dif > 0.f)
		hybridDirection *= -1.f;
	hybridPartition = max(0.f, min(1.f, hybridPartition + dif * 0.25f));


	lastFrameTime = frameTime;*/
	
	/*float dif = OptixData.lastTime - EmbreeData.lastTime;

	hybridPartition = max(0.f, min(1.f, hybridPartition + dif * 0.1f));*/

	hybridPartition = OptixData.avgTime / EmbreeData.avgTime - 0.5f;
	
	resize();

}