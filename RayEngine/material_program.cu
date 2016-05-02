#include "common.cuh"

rtBuffer<Light> lights;
rtTextureSampler<float4, 2> sampler;
rtDeclareVariable(float3, eye, , );
rtDeclareVariable(float4, ambient, , );
rtDeclareVariable(float4, diffuse, , );
rtDeclareVariable(float, shininess, , );
rtDeclareVariable(rtObject, sceneObj, , );

rtDeclareVariable(float3, normal, attribute normal, );
rtDeclareVariable(float2, texCoord, attribute texCoord, );

rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(RayColorData, rayColorData, rtPayload, );
rtDeclareVariable(RayShadowData, rayShadowData, rtPayload, );

RT_PROGRAM void anyHit() {

	rayShadowData.attenuation = 0.f;
	rtTerminateRay();

}

RT_PROGRAM void closestHit() {

	// Set hit properties
	float3 hitPos = ray.origin + ray.tmax * ray.direction;
	float3 toEye = normalize(eye - hitPos);

	// Calculate color
	float4 totalDiffuse, totalSpecular;
	totalDiffuse = totalSpecular = make_float4(0.f);

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
			optix::Ray shadowRay(hitPos, incidence, 1, 0.1f, distance);
			rtTrace(sceneObj, shadowRay, shadowData);

			if (shadowData.attenuation > 0.f) {

				attenuation *= shadowData.attenuation;

				// Diffuse factor
				float diffuse = max(dot(normal, incidence), 0.f) * attenuation;
				totalDiffuse += diffuse * light.color;

				// Specular factor
				if (shininess > 0.0) {
					float3 reflection = 2.f * dot(incidence, normal) * normal - incidence;
					float specular = pow(max(dot(reflection, toEye), 0.f), 1.f / shininess) * attenuation;
					totalSpecular += specular * light.color;
				}

			}

		}
	}

	// Create color
	float4 texColor = diffuse * tex2D(sampler, texCoord.x, texCoord.y);
	rayColorData.result = texColor * (ambient + totalDiffuse) + totalSpecular;
	rayColorData.result.w = texColor.w;

}
