#include "common.cuh"

rtTextureSampler<float4, 2> sky;

rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(RayColorData, curColorData, rtPayload, );

RT_PROGRAM void miss() {

	float3 nDir = normalize(ray.direction);
	float theta = atan2f(nDir.x, nDir.z);
	float phi = M_PIf * 0.5f - acosf(nDir.y);
	float u = (theta + M_PIf) * (0.5f * M_1_PIf);
	float v = 0.5f * (1.0f + sin(phi));
	curColorData.result = tex2D(sky, u, v);
}
