#include "rayengine.h"

void RayEngine::hybridInit() {

	Hybrid.partition = HYBRID_PARTITION;
	Hybrid.direction = -1.f;

}

void RayEngine::hybridRenderUpdatePartition() {

	if (Hybrid.balanceMode == BM_RENDER_TIME) {

		float dif = Optix.lastTime - Embree.lastTime;
		Hybrid.partition = max(0.f, min(1.f, Hybrid.partition + dif * 0.25f));
		resize(); // TODO: Does this take time?

	}

	/*if (Optix.avgTime == 0.f && Embree.avgTime == 0.f)
	return;

	static float lastFrameTime = 0.f;
	float frameTime = Optix.avgTime + Embree.avgTime;
	float dif = lastFrameTime - frameTime;

	if (dif > 0.f)
	hybridDirection *= -1.f;
	hybridPartition = max(0.f, min(1.f, hybridPartition + dif * 0.25f));


	lastFrameTime = frameTime;*/


}