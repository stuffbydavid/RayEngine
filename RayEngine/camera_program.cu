#include "common.cuh"

rtBuffer<float4, 2> renderBuffer;
rtDeclareVariable(float, offset, , );
rtDeclareVariable(float3, org, , );
rtDeclareVariable(float3, xaxis, , );
rtDeclareVariable(float3, yaxis, , );
rtDeclareVariable(float3, zaxis, , );
rtDeclareVariable(rtObject, sceneObj, , );

rtDeclareVariable(uint2, launchIndex, rtLaunchIndex, );
rtDeclareVariable(uint2, launchDim, rtLaunchDim, );

RT_PROGRAM void camera() {

	if (launchIndex.x < offset) {
		renderBuffer[launchIndex] = make_float4(0.f);
		return;
	}

	float2 d = (make_float2(launchIndex) / make_float2(launchDim)) * 2.f - 1.f;
	float3 rayOrg = org;
	float3 rayDir = d.x * xaxis + d.y * yaxis + zaxis;

	Ray ray = make_Ray(rayOrg, rayDir, 0, 0.1f, RT_DEFAULT_MAX);

	RayColorData data;
	data.reflectDepth = 0;
	data.refractDepth = 0;
	rtTrace(sceneObj, ray, data);

	renderBuffer[launchIndex] = data.result;

}