#include "common.cuh"
#include "random.cuh"

rtBuffer<Light> lights;
rtTextureSampler<float4, 2> sampler;
rtDeclareVariable(float3, org, , );
rtDeclareVariable(float4, ambient, , );
rtDeclareVariable(float4, specular, , );
rtDeclareVariable(float4, diffuse, , );
rtDeclareVariable(float, shineExponent, , );
rtDeclareVariable(float, reflectIntensity, , );
rtDeclareVariable(float, refractIndex, , );
rtDeclareVariable(float4, sceneAmbient, , );
rtDeclareVariable(rtObject, sceneObj, , );
rtDeclareVariable(int, maxReflections, , );
rtDeclareVariable(int, maxRefractions, , );
rtDeclareVariable(float, aoRadius, , );
rtTextureSampler<float4, 2> aoNoise;

rtDeclareVariable(float3, normal, attribute normal, );
rtDeclareVariable(float2, texCoord, attribute texCoord, );

rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(RayColorData, curColorData, rtPayload, );
rtDeclareVariable(RayShadowData, curShadowData, rtPayload, );

rtDeclareVariable(uint2, launchIndex, rtLaunchIndex, );
rtDeclareVariable(uint2, launchDim, rtLaunchDim, );

RT_PROGRAM void anyHit() {

	curShadowData.attenuation *= 1.f - diffuse.w * tex2D(sampler, texCoord.x, texCoord.y).w;

	if (curShadowData.attenuation == 0.f)
		rtTerminateRay();

}

RT_PROGRAM void closestHit() {

	// Set hit properties
	float3 hitPos = ray.origin + ray.tmax * ray.direction;
	float3 toEye = normalize(org - hitPos);

	// Calculate color
	float4 texture = diffuse * tex2D(sampler, texCoord.x, texCoord.y);
	float transparency = 1.f - texture.w;
	float4 totalDiffuse, totalSpecular, totalReflect, totalRefract;
	totalDiffuse = totalSpecular = totalReflect = totalRefract = make_float4(0.f);

	//// Light contribution ////

	for (int i = 0; i < lights.size(); i++) {

		Light light = lights[i];
		float3 incidence = normalize(light.position - hitPos);

		// Calculate attenuation (falloff)
		float distance = length(light.position - hitPos);
		float attenuation = max(1.f - distance / light.range, 0.f);

		if (attenuation > 0.0) {

			// Cast ray to find blocking objects
			RayShadowData shadowData;
			shadowData.attenuation = attenuation;
			Ray shadowRay(hitPos, incidence, 1, 0.1f, distance);
			rtTrace(sceneObj, shadowRay, shadowData);

			// The ray was not fully absorbed, add light contribution
			if (shadowData.attenuation > 0.f) {

				// Diffuse factor
				float diffuseFactor = max(dot(normal, incidence), 0.f) * shadowData.attenuation;
				totalDiffuse += diffuseFactor * light.color;

				// Specular factor
				if (shineExponent > 0.f) {
					float3 reflection = -reflect(incidence, normal);
					float specularFactor = pow(max(dot(reflection, toEye), 0.f), shineExponent) * shadowData.attenuation;
					totalSpecular += specularFactor * specular;
				}

			}

		}

	}

	//// Reflection ////

	if (reflectIntensity > 0.f && curColorData.reflectDepth < maxReflections) {

		float3 reflectVector = reflect(ray.direction, normal);

		RayColorData reflectData;
		reflectData.reflectDepth = curColorData.reflectDepth + 1;
		reflectData.refractDepth = curColorData.refractDepth;

		Ray reflectRay(hitPos, reflectVector, 0, 0.1f);
		rtTrace(sceneObj, reflectRay, reflectData);
		totalReflect = reflectData.result * reflectIntensity;

	}

	//// Refraction ////

	if (transparency > 0.f && curColorData.refractDepth < maxRefractions) {

		float3 refractVector;
		if (!refract(refractVector, ray.direction, normal, refractIndex))
			refractVector = ray.direction;

		RayColorData refractData;
		refractData.reflectDepth = curColorData.reflectDepth;
		refractData.refractDepth = curColorData.refractDepth + 1;

		Ray refractRay(hitPos, refractVector, 0, 0.1f);
		rtTrace(sceneObj, refractRay, refractData);
		totalRefract = refractData.result * transparency;

	}

	/// Ambient occlusion ////

	float2 pos = (make_float2(launchIndex) / make_float2(launchDim)) * 20.f;
	float4 noise = tex2D(aoNoise, pos.x, pos.y);
	float occluded = 0.f;
	Onb onb(normal);

	int samples = 16;
	float invSamples = 1.f / samples;
	for (int i = 0; i < samples; i++) {

		float u1 = (float(i % 4) + noise.x) * (1.f / 5.f);
		float u2 = (float(i / 4) + noise.y) * (1.f / 5.f);
		float3 sampleVector;
		cosine_sample_hemisphere(u1, u2, sampleVector);
		onb.inverse_transform(sampleVector);

		RayShadowData sampleData;
		sampleData.attenuation = 1.f;
		Ray sampleRay(hitPos, sampleVector, 1, 0.01f, aoRadius);
		rtTrace(sceneObj, sampleRay, sampleData);

		occluded += 1.f - sampleData.attenuation;

	}

	float aoPower = 2.f;
	occluded = 1.f - pow(1.f - occluded * invSamples, aoPower);

	// Create color
	curColorData.result = texture * (sceneAmbient + ambient + totalDiffuse) * (1.f - occluded) * (1.f - transparency) + totalSpecular + totalReflect + totalRefract;
	curColorData.result.w = 1.f;

}
