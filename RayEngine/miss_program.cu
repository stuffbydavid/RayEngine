#include "common.cuh"

rtDeclareVariable(float4, backgroundColor, , );

rtDeclareVariable(RayData, rayData, rtPayload, );

RT_PROGRAM void miss() {
	rayData.result = backgroundColor;
}
