#include "common.cuh"
#include "random.cuh"

rtDeclareVariable(float, offset, , );
rtDeclareVariable(float, windowWidth, , );
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
rtDeclareVariable(int, enableReflections, , );
rtDeclareVariable(int, maxReflections, , );
rtDeclareVariable(int, enableRefractions, , );
rtDeclareVariable(int, maxRefractions, , );
rtDeclareVariable(int, enableAo, , );
rtDeclareVariable(float, aoRadius, , );
rtDeclareVariable(float, aoPower, , );
rtDeclareVariable(int, aoSamples, , );
rtDeclareVariable(int, aoSamplesSqrt, , );
rtTextureSampler<float4, 2> aoNoise;
rtDeclareVariable(float, aoNoiseScale, , );

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
			Ray shadowRay(hitPos, incidence, 1, 0.01f, distance);
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

	if (enableReflections && reflectIntensity > 0.f && curColorData.reflectDepth < maxReflections) {

		float3 reflectVector = reflect(ray.direction, normal);

		RayColorData reflectData;
		reflectData.reflectDepth = curColorData.reflectDepth + 1;
		reflectData.refractDepth = curColorData.refractDepth;

		Ray reflectRay(hitPos, reflectVector, 0, 0.01f);
		rtTrace(sceneObj, reflectRay, reflectData);
		totalReflect = reflectData.result * reflectIntensity;

	}

	//// Refraction ////

	if (enableRefractions && transparency > 0.f && curColorData.refractDepth < maxRefractions) {

		float3 refractVector;
		if (!refract(refractVector, ray.direction, normal, refractIndex))
			refractVector = ray.direction;

		RayColorData refractData;
		refractData.reflectDepth = curColorData.reflectDepth;
		refractData.refractDepth = curColorData.refractDepth + 1;

		Ray refractRay(hitPos, refractVector, 0, 0.01f);
		rtTrace(sceneObj, refractRay, refractData);
		totalRefract = refractData.result * transparency;

	}

	/// Ambient occlusion ////

	float occluded = 0.f;

	if (enableAo) {

		float invSamples = 1.f / aoSamples;
		float invSamplesSqrt = 1.f / aoSamplesSqrt;
		float2 noiseTexCoord = (make_float2(offset + launchIndex.x, launchIndex.y) / make_float2(windowWidth, launchDim.y)) * aoNoiseScale;
		float4 noise = tex2D(aoNoise, noiseTexCoord.x, noiseTexCoord.y);
		Onb onb(normal);

		for (int i = 0; i < aoSamples; i++) {

			float u1 = (float(i % aoSamplesSqrt) + noise.x) * invSamplesSqrt;
			float u2 = (float(i / aoSamplesSqrt) + noise.y) * invSamplesSqrt;
			float3 sampleVector;
			cosine_sample_hemisphere(u1, u2, sampleVector);
			onb.inverse_transform(sampleVector);

			RayShadowData sampleData;
			sampleData.attenuation = 1.f;
			Ray sampleRay(hitPos, sampleVector, 1, 0.01f, aoRadius);
			rtTrace(sceneObj, sampleRay, sampleData);

			occluded += 1.f - sampleData.attenuation;

		}

		occluded *= invSamples * aoPower;

	}

	// Create color
	curColorData.result = texture * (sceneAmbient + ambient + totalDiffuse) * (1.f - occluded) * (1.f - transparency) + totalSpecular + totalReflect + totalRefract;
	curColorData.result.w = 1.f;

}
