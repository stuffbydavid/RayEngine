#include "common.cuh"

rtDeclareVariable(float4, background, , );

rtDeclareVariable(RayColorData, rayColorData, rtPayload, );

RT_PROGRAM void miss() {
	rayColorData.result = background;
}
