#include "common.cuh"

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

rtDeclareVariable(float3, normal, attribute normal, );
rtDeclareVariable(float2, texCoord, attribute texCoord, );

rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(RayColorData, curColorData, rtPayload, );
rtDeclareVariable(RayShadowData, curShadowData, rtPayload, );

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
			shadowData.attenuation = 1.f;
			Ray shadowRay(hitPos, incidence, 1, 0.01f, distance);
			rtTrace(sceneObj, shadowRay, shadowData);

			// The ray was not fully absorbed, add light contribution
			if (shadowData.attenuation > 0.f) {

				attenuation *= shadowData.attenuation;

				// Diffuse factor
				float diffuseFactor = max(dot(normal, incidence), 0.f) * attenuation;
				totalDiffuse += diffuseFactor * light.color;

				// Specular factor
				if (shineExponent > 0.0) {
					float3 reflection = -reflect(incidence, normal);
					float specularFactor = pow(max(dot(reflection, toEye), 0.f), shineExponent) * attenuation;
					totalSpecular += specularFactor * specular;
				}

			}

		}
	}

	//// Reflection ////

	if (reflectIntensity > 0.f && curColorData.reflectDepth < maxReflections) {
		RayColorData reflectData;
		reflectData.reflectDepth = curColorData.reflectDepth + 1;
		Ray reflectRay(hitPos, reflect(ray.direction, normal), 0, 0.01f);
		rtTrace(sceneObj, reflectRay, reflectData);
		totalReflect = reflectData.result * reflectIntensity; // TODO: Use material reflectiveness
	}

	//// Refraction ////

	if (transparency > 0.f && curColorData.refractDepth < maxRefractions) {

		float3 refractVector;
		if (!refract(refractVector, ray.direction, normal, refractIndex))
			refractVector = ray.direction;

		RayColorData refractData;
		refractData.refractDepth = curColorData.refractDepth+ 1;
		Ray refractRay(hitPos, refractVector, 0, 0.01f);
		rtTrace(sceneObj, refractRay, refractData);
		totalRefract = refractData.result * transparency;

	}

	// Create color
	curColorData.result = texture * (sceneAmbient + ambient + totalDiffuse) * (1.f - transparency) + totalSpecular + totalReflect + totalRefract;
	//curColorData.result.w = 1.f;

}
