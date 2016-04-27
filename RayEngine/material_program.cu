#include "common.cuh"

rtDeclareVariable(float3, normal, attribute normal, );
rtDeclareVariable(RayData, rayData, rtPayload, );

RT_PROGRAM void closestHit() {
	rayData.result = normal * 0.5f + 0.5f;
}
