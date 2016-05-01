#include "common.cuh"

rtTextureSampler<float4, 2> sampler;
rtDeclareVariable(float3, normal, attribute normal, );
rtDeclareVariable(float2, texCoord, attribute texCoord, );
rtDeclareVariable(RayData, rayData, rtPayload, );

RT_PROGRAM void closestHit() {
	rayData.result = make_float3(tex2D(sampler, texCoord.x, texCoord.y));
}
