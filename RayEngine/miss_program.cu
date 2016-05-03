#include "common.cuh"

rtDeclareVariable(float4, background, , );

rtDeclareVariable(RayColorData, curColorData, rtPayload, );

RT_PROGRAM void miss() {
	curColorData.result = background;
}
