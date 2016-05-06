#include "common.cuh"

rtBuffer<Light> lights;
rtTextureSampler<float4, 2> sampler;
rtDeclareVariable(float3, eye, , );
rtDeclareVariable(float4, ambient, , );
rtDeclareVariable(float4, specular, , );
rtDeclareVariable(float4, diffuse, , );
rtDeclareVariable(float, shininess, , );
rtDeclareVariable(float4, sceneAmbient, , );
rtDeclareVariable(rtObject, sceneObj, , );
rtDeclareVariable(int, maxReflections, , );

rtDeclareVariable(float3, normal, attribute normal, );
rtDeclareVariable(float2, texCoord, attribute texCoord, );

rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(RayColorData, curColorData, rtPayload, );
rtDeclareVariable(RayShadowData, curShadowData, rtPayload, );

RT_PROGRAM void anyHit() {

	curShadowData.attenuation = 0.f;
	rtTerminateRay();

}

RT_PROGRAM void closestHit() {

	// Set hit properties
	float3 hitPos = ray.origin + ray.tmax * ray.direction;
	float3 toEye = normalize(eye - hitPos);

	// Calculate color
	float4 totalDiffuse, totalSpecular, totalReflect;
	totalDiffuse = totalSpecular = totalReflect = make_float4(0.f);

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
				if (shininess > 0.0) {
					float3 reflection = -reflect(incidence, normal);
					float specularFactor = pow(max(dot(reflection, toEye), 0.f), shininess) * attenuation;
					totalSpecular += specularFactor * specular;
				}

			}

		}
	}

	// Reflection
	if (curColorData.depth < maxReflections) {
		RayColorData reflectData;
		reflectData.depth = curColorData.depth + 1;
		Ray reflectRay(hitPos, reflect(ray.direction, normal), 0, 0.01f);
		rtTrace(sceneObj, reflectRay, reflectData);
		totalReflect = reflectData.result * make_float4(0.25f);
	}

	// Create color
	float4 texColor = diffuse * tex2D(sampler, texCoord.x, texCoord.y);
	curColorData.result = texColor * (sceneAmbient + ambient + totalDiffuse) + totalSpecular + totalReflect;

}
