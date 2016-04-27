#include "common.cuh"

rtDeclareVariable(float3, bgColor, , );
rtDeclareVariable(RayData, rayData, rtPayload, );

RT_PROGRAM void miss() {
	rayData.result = bgColor;
}
