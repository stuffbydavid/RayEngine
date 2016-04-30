#include "common.cuh"

rtDeclareVariable(float, offset, , );
rtDeclareVariable(float, windowWidth, , );
rtDeclareVariable(float3, eye, , );
rtDeclareVariable(float3, xaxis, , );
rtDeclareVariable(float3, yaxis, , );
rtDeclareVariable(float3, zaxis, , );
rtBuffer<float4, 2> outputBuffer;
rtDeclareVariable(rtObject, sceneObj, , );

rtDeclareVariable(uint2, launchIndex, rtLaunchIndex, );
rtDeclareVariable(uint2, launchDim, rtLaunchDim, );

RT_PROGRAM void camera() {

	float2 d = (make_float2(offset + launchIndex.x, launchIndex.y) / make_float2(windowWidth, launchDim.y)) * 2.f - 1.f;
	float3 rayOrg = eye;
	float3 rayDir = d.x * xaxis + d.y * yaxis + zaxis;

	optix::Ray ray = make_Ray(rayOrg, rayDir, 0, 0.01f, RT_DEFAULT_MAX);

	RayData rd;
	rtTrace(sceneObj, ray, rd);

	outputBuffer[launchIndex] = make_float4(rd.result, 1.f);
}