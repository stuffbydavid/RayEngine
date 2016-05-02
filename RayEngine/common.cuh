#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>
#include <optixu/optixu_aabb_namespace.h>

using namespace optix;

struct RayColorData {
	float4 result;
};

struct RayShadowData {
	float attenuation;
};

struct Light {
	float3 position;
	float4 color;
	float range;
};