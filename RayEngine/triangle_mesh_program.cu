#include "common.cuh"

rtBuffer<float3, 1> posData;
rtBuffer<float3, 1> normalData;
rtBuffer<float2, 1> texCoordData;
rtBuffer<uint3, 1> indexData;

rtDeclareVariable(float3, normal, attribute normal, );
rtDeclareVariable(float2, texCoord, attribute texCoord, );

rtDeclareVariable(Ray, ray, rtCurrentRay, );

RT_PROGRAM void intersect(int primId) {

	uint3 prim = indexData[primId];
	float3 p0 = posData[prim.x];
	float3 p1 = posData[prim.y];
	float3 p2 = posData[prim.z];

	float3 n;
	float t, u, v;

	if (intersect_triangle(ray, p0, p1, p2, n, t, u, v)) {

		if (rtPotentialIntersection(t)) {

			// Normal
			float3 n0 = normalData[prim.x];
			float3 n1 = normalData[prim.y];
			float3 n2 = normalData[prim.z];
			normal = normalize(rtTransformVector(RT_OBJECT_TO_WORLD, n0 * (1.f - u - v) + n1 * u + n2 * v));

			// Texture coordinate
			float2 t0 = texCoordData[prim.x];
			float2 t1 = texCoordData[prim.y];
			float2 t2 = texCoordData[prim.z];
			texCoord = t0 * (1.f - u - v) + t1 * u + t2 * v;

			rtReportIntersection(0);

		}

	}

}

RT_PROGRAM void bounds(int primId, float result[6]) {

	uint3 prim = indexData[primId];
	float3 p0 = posData[prim.x];
	float3 p1 = posData[prim.y];
	float3 p2 = posData[prim.z];

	optix::Aabb* aabb = (optix::Aabb*)result;
	aabb->m_min = fminf(fminf(p0, p1), p2);
	aabb->m_max = fmaxf(fmaxf(p0, p1), p2);

}