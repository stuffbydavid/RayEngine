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

rtDeclareVariable(float3, normal, attribute normal, );
rtDeclareVariable(float2, texCoord, attribute texCoord, );

rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(RayColorData, curColorData, rtPayload, );
rtDeclareVariable(RayShadowData, curShadowData, rtPayload, );

RT_PROGRAM void anyHit() {

	float transColor = diffuse.w * tex2D(sampler, texCoord.x, texCoord.y).w;
	curShadowData.attenuation *= 1.f - transColor;

	if (curShadowData.attenuation == 0.f)
		rtTerminateRay();

}

RT_PROGRAM void closestHit() {

	// Set hit properties
	float3 hitPos = ray.origin + ray.tmax * ray.direction;
	float3 toEye = normalize(org - hitPos);

	// Calculate color
	float4 totalDiffuse, totalSpecular, totalReflect, totalRefract;
	totalDiffuse = totalSpecular = totalReflect = totalRefract = make_float4(0.f);

	// Go through the lights
	for (int i = 0; i < lights.size(); i++) {
		Light light = lights[i];
		float3 incidence = normalize(light.position - hitPos);

		// Calculate attenuation (falloff)
		float distance = length(light.position - hitPos);
		float attenuation = max(1.f - distance / light.range, 0.f);

		if (attenuation > 0.0) {

			// Cast shadow ray
			RayShadowData shadowData = { 1.f };
			Ray shadowRay(hitPos, incidence, 1, 0.01f, distance);
			rtTrace(sceneObj, shadowRay, shadowData);

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

	// Reflection
	if (reflectIntensity > 0.f && curColorData.depth < maxReflections) {
		RayColorData reflectData;
		reflectData.depth = curColorData.depth + 1;
		Ray reflectRay(hitPos, reflect(ray.direction, normal), 0, 0.01f);
		rtTrace(sceneObj, reflectRay, reflectData);
		totalReflect = reflectData.result * reflectIntensity; // TODO: Use material reflectiveness
	}

	// Refract
	float transparency = diffuse.w * tex2D(sampler, texCoord.x, texCoord.y).w;
	if (transparency < 1.f) {

		float3 refractVector;
		if (!refract(refractVector, ray.direction, normal, refractIndex))
			refractVector = ray.direction;

		RayColorData refractData;
		refractData.depth = curColorData.depth;
		Ray refractRay(hitPos, refractVector, 0, 0.01f);
		rtTrace(sceneObj, refractRay, refractData);
		totalRefract = refractData.result * (1.f - transparency);

	}

	// Create color
	float4 texColor = diffuse * tex2D(sampler, texCoord.x, texCoord.y);
	curColorData.result = texColor * (sceneAmbient + ambient + totalDiffuse) + totalSpecular + totalReflect + totalRefract;


}
