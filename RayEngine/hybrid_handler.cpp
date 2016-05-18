#include "rayengine.h"

void RayEngine::hybridInit() {

	Hybrid.partition = HYBRID_PARTITION;
	Hybrid.direction = -1.f;

}

void RayEngine::hybridRenderUpdatePartition() {

	if (Hybrid.balanceMode == BM_RENDER_TIME) {

		float dif = Optix.renderTimer.lastTime - Embree.renderTimer.lastTime;
		float lastPartition = Hybrid.partition;
		Hybrid.partition = max(0.f, min(1.f, Hybrid.partition + dif * 0.25f));

		if (abs(lastPartition - Hybrid.partition) < 0.05) {
			embreeUpdatePartition();
			optixUpdatePartition();
		}

	}

}