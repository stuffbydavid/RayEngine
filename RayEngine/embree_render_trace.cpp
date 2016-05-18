#include "rayengine.h"

// Stores the properties of a ray hit
struct RayHit {

	Color texture, diffuse, specular;
	float transparency, occluded;
	Vec3 pos, normal;
	Vec2 texCoord;
	Object* obj;
	TriangleMesh* mesh;
	Material* material;
	bool hitSky;

};

// Returns the color of missed rays
Color RayEngine::embreeRenderSky(Vec3 dir) {

	Vec3 nDir = Vec3::normalize(dir);
	float theta = atan2f(nDir.x(), nDir.z());
	float phi = M_PIf * 0.5f - acosf(nDir.y());
	float u = (theta + M_PIf) * (0.5f * M_1_PIf);
	float v = 0.5f * (1.0f + sin(phi));
	return curScene->sky->getPixel(Vec2(u, v));

}

void RayEngine::embreeOcclusionFilter(void* data, Embree::LightRay& ray) {
	
	if (ray.geomID == RTC_INVALID_GEOMETRY_ID)
		return;

	// Store hit
	Object* obj = ((RayEngine*)data)->curScene->Embree.instIDmap[ray.instID];
	TriangleMesh* mesh = (TriangleMesh*)obj->Embree.geomIDmap[ray.geomID];
	Material* material = mesh->material;
	Vec2 texCoord = mesh->getTexCoord(ray.primID, ray.u, ray.v);
	
	// Multiply by transparency
	ray.attenuation *= 1.f - material->diffuse.a() * material->image->getPixel(texCoord).a();

	// Keep going
	if (ray.attenuation > 0.f)
		ray.geomID = RTC_INVALID_GEOMETRY_ID;

}

void RayEngine::embreeOcclusionFilter8(int* valid, void* data, Embree::LightRayPacket& packet) {

	for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

		// Invalid or already completely absorbed
		if (valid[i] == EMBREE_RAY_INVALID || packet.geomID[i] == RTC_INVALID_GEOMETRY_ID || packet.attenuation[i] == 0.f)
			continue;

		// Store hit
		Object* obj = ((RayEngine*)data)->curScene->Embree.instIDmap[packet.instID[i]];
		TriangleMesh* mesh = (TriangleMesh*)obj->Embree.geomIDmap[packet.geomID[i]];
		Material* material = mesh->material;
		Vec2 texCoord = mesh->getTexCoord(packet.primID[i], packet.u[i], packet.v[i]);

		// Multiply by transparency
		packet.attenuation[i] *= 1.f - material->diffuse.a() * material->image->getPixel(texCoord).a();

		// Keep going
		if (packet.attenuation[i] > 0.f)
			packet.geomID[i] = RTC_INVALID_GEOMETRY_ID;

	}

}

void RayEngine::embreeRenderTraceRay(Embree::Ray& ray, int reflectDepth, int refractDepth, Color& result) {

	// No object hit, return sky color
	if (ray.geomID == RTC_INVALID_GEOMETRY_ID) {
		result = embreeRenderSky(ray.dir);
		return;
	}

	// Store hit
	RayHit hit;
	hit.diffuse = hit.specular = { 0.f };
	hit.pos = Vec3(ray.org) + Vec3(ray.dir) * ray.tfar;
	hit.obj = curScene->Embree.instIDmap[ray.instID];
	hit.mesh = (TriangleMesh*)hit.obj->Embree.geomIDmap[ray.geomID];
	hit.material = hit.mesh->material;
	hit.normal = Vec3::normalize(hit.obj->matrix * hit.mesh->getNormal(ray.primID, ray.u, ray.v));
	hit.texCoord = hit.mesh->getTexCoord(ray.primID, ray.u, ray.v);
	hit.texture = hit.material->diffuse * hit.material->image->getPixel(hit.texCoord);
	hit.transparency = 1.f - hit.texture.a();

	// Check lights
#if EMBREE_ONE_LIGHT
	Light& light = curScene->lights[0];
#else
	for (int l = 0; l < curScene->lights.size(); l++) {
		Light& light = curScene->lights[l];
 #endif

		float distance = Vec3::length(light.position - hit.pos);
		float attenuation = max(1.f - distance / light.range, 0.f);

		// Light is in reach
		if (attenuation > 0.f) {

			// Define light ray
			Vec3 incidence = Vec3::normalize(light.position - hit.pos);
			Embree::LightRay lRay;
			lRay.org[0] = hit.pos.x();
			lRay.org[1] = hit.pos.y();
			lRay.org[2] = hit.pos.z();
			lRay.dir[0] = incidence.x();
			lRay.dir[1] = incidence.y();
			lRay.dir[2] = incidence.z();
			lRay.tnear = 0.01f;
			lRay.tfar = distance;
			lRay.instID =
			lRay.geomID =
			lRay.primID = RTC_INVALID_GEOMETRY_ID;
			lRay.mask = EMBREE_RAY_VALID;
			lRay.time = 0.f;
			lRay.attenuation = attenuation;

			// Check occlusion
			rtcOccluded(curScene->Embree.scene, lRay);

			// The ray was not fully absorbed, add light contribution
			if (lRay.attenuation > 0.f) {

				// Diffuse factor
				float diffuseFactor = max(Vec3::dot(hit.normal, incidence), 0.f) * lRay.attenuation;
				hit.diffuse += diffuseFactor * light.color;

				// Specular factor
				if (hit.material->shineExponent > 0.f) {
					Vec3 toEye = Vec3::normalize(curCamera->position - hit.pos);
					Vec3 reflection = Vec3::reflect(incidence, hit.normal);
					float specularFactor = pow(max(Vec3::dot(reflection, toEye), 0.f), hit.material->shineExponent) * lRay.attenuation;
					hit.specular += specularFactor * hit.material->specular;
				}

			}

		}

#if !(EMBREE_ONE_LIGHT)
	}
#endif

	// Create color
	result = hit.texture * (curScene->ambient + hit.material->ambient + hit.diffuse) * (1.f - hit.transparency) + hit.specular;

	// Add reflections
	if (enableReflections && hit.material->reflectIntensity > 0.f && reflectDepth < maxReflections) {

		Vec3 reflDir = Vec3::reflect(-Vec3(ray.dir), hit.normal);

		Embree::Ray rRay;
		rRay.org[0] = hit.pos.x();
		rRay.org[1] = hit.pos.y();
		rRay.org[2] = hit.pos.z();
		rRay.dir[0] = reflDir.x();
		rRay.dir[1] = reflDir.y();
		rRay.dir[2] = reflDir.z();
		rRay.tnear = 0.01f;
		rRay.tfar = FLT_MAX;
		rRay.instID =
		rRay.geomID =
		rRay.primID = RTC_INVALID_GEOMETRY_ID;
		rRay.mask = EMBREE_RAY_VALID;
		rRay.time = 0.f;

		rtcIntersect(curScene->Embree.scene, rRay);

		Color reflectResult;
		embreeRenderTraceRay(rRay, reflectDepth + 1, refractDepth, reflectResult);

		result += reflectResult * hit.material->reflectIntensity;

	}

	// Add refractions
	if (enableRefractions && hit.transparency > 0.f && refractDepth < maxRefractions) {

		Vec3 refrDir = Vec3::refract(ray.dir, hit.normal, hit.material->refractIndex);

		Embree::Ray rRay;
		rRay.org[0] = hit.pos.x();
		rRay.org[1] = hit.pos.y();
		rRay.org[2] = hit.pos.z();
		rRay.dir[0] = refrDir.x();
		rRay.dir[1] = refrDir.y();
		rRay.dir[2] = refrDir.z();
		rRay.tnear = 0.01f;
		rRay.tfar = FLT_MAX;
		rRay.instID =
		rRay.geomID =
		rRay.primID = RTC_INVALID_GEOMETRY_ID;
		rRay.mask = EMBREE_RAY_VALID;
		rRay.time = 0.f;

		rtcIntersect(curScene->Embree.scene, rRay);

		Color refractResult;
		embreeRenderTraceRay(rRay, reflectDepth, refractDepth + 1, refractResult);

		result += refractResult * hit.transparency;

	}

	result.a(1);

}

void RayEngine::embreeRenderTracePacket(Embree::RayPacket& packet, int reflectDepth, int refractDepth, Color* result) {
	
#if EMBREE_ONE_LIGHT

	Embree::LightRayPacket lightPacket;
	for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
		lightPacket.valid[i] = EMBREE_RAY_INVALID;

#else

    #define MAX_LIGHTS 2

	// Create light packets (invalid by default)
	int numLights = min((int)curScene->lights.size(), MAX_LIGHTS - 1);
	Embree::LightRayPacket lightPackets[MAX_LIGHTS];
	for (int l = 0; l < MAX_LIGHTS; l++)
		for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
			lightPackets[l].valid[i] = EMBREE_RAY_INVALID;

#endif

	// Reflection packet (invalid by default)
	bool doReflections = false;
	Embree::RayPacket reflectPacket;
	Color reflectResult[EMBREE_PACKET_SIZE];
	if (enableReflections) {
		reflectPacket.x = packet.x;
		reflectPacket.y = packet.y;
		for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
			reflectPacket.valid[i] = EMBREE_RAY_INVALID;
	}

	// Refraction packet (invalid by default)
	bool doRefractions = false;
	Embree::RayPacket refractPacket;
	Color refractResult[EMBREE_PACKET_SIZE];
	if (enableRefractions) {
		refractPacket.x = packet.x;
		refractPacket.y = packet.y;
		for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
			refractPacket.valid[i] = EMBREE_RAY_INVALID;
	}

	// Ambient occlusion packets
	Embree::LightRayPacket aoPackets[AO_SAMPLES_MAX];
	float invSamples = 1.f / aoSamples;
	float invSamplesSqrt = 1.f / aoSamplesSqrt;
	if (enableAo)
		for (int a = 0; a < AO_SAMPLES_MAX; a++)
			for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
				aoPackets[a].valid[i] = EMBREE_RAY_INVALID;
	
	//// Store hits ////
	
	RayHit hits[EMBREE_PACKET_SIZE];
	
	for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

		if (packet.valid[i] == EMBREE_RAY_INVALID)
			continue;

		RayHit& hit = hits[i];
		Vec3 rayOrg = Vec3(packet.orgx[i], packet.orgy[i], packet.orgz[i]);
		Vec3 rayDir = Vec3(packet.dirx[i], packet.diry[i], packet.dirz[i]);

		if (packet.geomID[i] == RTC_INVALID_GEOMETRY_ID) {
			result[i] = embreeRenderSky(rayDir);
			hit.hitSky = true;
			continue;
		}

		// Store hit properties
		hit.diffuse = hit.specular = { 0.f };
		hit.pos = rayOrg + rayDir * packet.tfar[i];
		hit.obj = curScene->Embree.instIDmap[packet.instID[i]];
		hit.mesh = (TriangleMesh*)hit.obj->Embree.geomIDmap[packet.geomID[i]];
		hit.material = hit.mesh->material;
		hit.normal = Vec3::normalize(hit.obj->matrix * hit.mesh->getNormal(packet.primID[i], packet.u[i], packet.v[i]));
		hit.texCoord = hit.mesh->getTexCoord(packet.primID[i], packet.u[i], packet.v[i]);
		hit.texture = hit.material->diffuse * hit.material->image->getPixel(hit.texCoord);
		hit.transparency = 1.f - hit.texture.a();
		hit.occluded = 0.f;
		hit.hitSky = false;

		// Check lights
#if EMBREE_ONE_LIGHT
		Light& light = curScene->lights[0];
#else
		for (int l = 0; l < numLights; l++) {
			Light& light = curScene->lights[l];
#endif

			float distance = Vec3::length(light.position - hit.pos);
			float attenuation = max(1.f - distance / light.range, 0.f);

			// Light is in reach
			if (attenuation > 0.f) {

				Vec3 incidence = Vec3::normalize(light.position - hit.pos);

				// Define light ray
#if EMBREE_ONE_LIGHT
				Embree::LightRayPacket& lPacket = lightPacket;
#else
				Embree::LightRayPacket& lPacket = lightPackets[l];
#endif
				lPacket.attenuation[i] = attenuation;
				lPacket.distance[i] = distance;
				lPacket.incidence[i] = incidence;
				lPacket.orgx[i] = hit.pos.x();
				lPacket.orgy[i] = hit.pos.y();
				lPacket.orgz[i] = hit.pos.z();
				lPacket.dirx[i] = incidence.x();
				lPacket.diry[i] = incidence.y();
				lPacket.dirz[i] = incidence.z();
				lPacket.tnear[i] = 0.01f;
				lPacket.tfar[i] = distance;
				lPacket.instID[i] =
				lPacket.geomID[i] =
				lPacket.primID[i] = RTC_INVALID_GEOMETRY_ID;
				lPacket.mask[i] = EMBREE_RAY_VALID;
				lPacket.time[i] = 0.f;
				lPacket.valid[i] = EMBREE_RAY_VALID;

			}

#if !(EMBREE_ONE_LIGHT)
		}
#endif

		// Create reflection rays
		if (enableReflections && hit.material->reflectIntensity > 0.f && reflectDepth < maxReflections) {

			Vec3 reflDir = Vec3::reflect(-rayDir, hit.normal);
			reflectPacket.orgx[i] = hit.pos.x();
			reflectPacket.orgy[i] = hit.pos.y();
			reflectPacket.orgz[i] = hit.pos.z();
			reflectPacket.dirx[i] = reflDir.x();
			reflectPacket.diry[i] = reflDir.y();
			reflectPacket.dirz[i] = reflDir.z();
			reflectPacket.tnear[i] = 0.01f;
			reflectPacket.tfar[i] = FLT_MAX;
			reflectPacket.instID[i] =
			reflectPacket.geomID[i] =
			reflectPacket.primID[i] = RTC_INVALID_GEOMETRY_ID;
			reflectPacket.mask[i] = EMBREE_RAY_VALID;
			reflectPacket.valid[i] = EMBREE_RAY_VALID;
			reflectPacket.time[i] = 0.f;
			doReflections = true;

		}

		// Create refraction rays
		if (enableRefractions && hit.transparency > 0.f && refractDepth < maxRefractions) {

			Vec3 refrDir = Vec3::refract(rayDir, hit.normal, hit.material->refractIndex);
			refractPacket.orgx[i] = hit.pos.x();
			refractPacket.orgy[i] = hit.pos.y();
			refractPacket.orgz[i] = hit.pos.z();
			refractPacket.dirx[i] = refrDir.x();
			refractPacket.diry[i] = refrDir.y();
			refractPacket.dirz[i] = refrDir.z();
			refractPacket.tnear[i] = 0.01f;
			refractPacket.tfar[i] = FLT_MAX;
			refractPacket.instID[i] =
			refractPacket.geomID[i] =
			refractPacket.primID[i] = RTC_INVALID_GEOMETRY_ID;
			refractPacket.mask[i] = EMBREE_RAY_VALID;
			refractPacket.valid[i] = EMBREE_RAY_VALID;
			refractPacket.time[i] = 0.f;
			doRefractions = true;

		}

		// Create ambient occlusion samples
		if (enableAo) {

			Color noise = aoNoiseImage->getPixel(Vec2((float)(Embree.offset + packet.x + i) / window.width, (float)packet.y / window.height) * aoNoiseScale);
			optix::Onb onb(optix::make_float3(hit.normal.x(), hit.normal.y(), hit.normal.z()));

			for (int a = 0; a < aoSamples; a++) {

				float u1 = (float(a % aoSamplesSqrt) + noise.r()) * invSamplesSqrt;
				float u2 = (float(a / aoSamplesSqrt) + noise.g()) * invSamplesSqrt;
				optix::float3 sampleVector;
				cosine_sample_hemisphere(u1, u2, sampleVector);
				onb.inverse_transform(sampleVector);

				Embree::LightRayPacket& aoPacket = aoPackets[a];
				aoPacket.attenuation[i] = 1.f;
				aoPacket.orgx[i] = hit.pos.x();
				aoPacket.orgy[i] = hit.pos.y();
				aoPacket.orgz[i] = hit.pos.z();
				aoPacket.dirx[i] = sampleVector.x;
				aoPacket.diry[i] = sampleVector.y;
				aoPacket.dirz[i] = sampleVector.z;
				aoPacket.tnear[i] = 0.01f;
				aoPacket.tfar[i] = curScene->aoRadius;
				aoPacket.instID[i] =
				aoPacket.geomID[i] =
				aoPacket.primID[i] = RTC_INVALID_GEOMETRY_ID;
				aoPacket.mask[i] = EMBREE_RAY_VALID;
				aoPacket.time[i] = 0.f;
				aoPacket.valid[i] = EMBREE_RAY_VALID;

			}

		}
		
	}
	
	//// Light pass ////

#if EMBREE_ONE_LIGHT

	Light& light = curScene->lights[0];
	Embree::LightRayPacket& lPacket = lightPacket;
	rtcOccluded8(lightPacket.valid, curScene->Embree.scene, lightPacket);

#else

	for (int l = 0; l < numLights; l++) {

		Light& light = curScene->lights[l];
		Embree::LightRayPacket& lPacket = lightPackets[l];

		rtcOccluded8(lPacket.valid, curScene->Embree.scene, lPacket);

#endif

		for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

			// Light ray was invalid or fully absorbed
			if (lPacket.valid[i] == EMBREE_RAY_INVALID || lPacket.attenuation[i] == 0.f)
				continue;

			RayHit& hit = hits[i];

			// Diffuse factor
			float diffuseFactor = max(Vec3::dot(hit.normal, lPacket.incidence[i]), 0.f) * lPacket.attenuation[i];
			hit.diffuse += diffuseFactor * light.color;

			// Specular factor
			if (hit.material->shineExponent > 0.0) {
				Vec3 toEye = Vec3::normalize(curCamera->position - hit.pos);
				Vec3 reflection = Vec3::reflect(lPacket.incidence[i], hit.normal);
				float specularFactor = pow(max(Vec3::dot(reflection, toEye), 0.f), hit.material->shineExponent) * lPacket.attenuation[i];
				hit.specular += specularFactor * hit.material->specular;
			}

		}

#if !(EMBREE_ONE_LIGHT)
	}
#endif

	//// Reflections ////

	if (doReflections) {

		rtcIntersect8(reflectPacket.valid, curScene->Embree.scene, reflectPacket);
		embreeRenderTracePacket(reflectPacket, reflectDepth + 1, refractDepth, reflectResult);

	}

	//// Refractions ////

	if (doRefractions) {

		rtcIntersect8(refractPacket.valid, curScene->Embree.scene, refractPacket);
		embreeRenderTracePacket(refractPacket, reflectDepth, refractDepth + 1, refractResult);

	}

	// Ambient occlusion

	if (enableAo) {

		for (int a = 0; a < aoSamples; a++) {

			rtcOccluded8(aoPackets[a].valid, curScene->Embree.scene, aoPackets[a]);

			for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
				if (aoPackets[a].valid[i] == EMBREE_RAY_VALID)
					hits[i].occluded += 1.f - aoPackets[a].attenuation[i];

		}

	}

	//// Calculate hit colors ////

	for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

		RayHit& hit = hits[i];

		if (packet.valid[i] == EMBREE_RAY_INVALID || hit.hitSky)
			continue;

		// Create color
		result[i] = hit.texture * (curScene->ambient + hit.material->ambient + hit.diffuse) * (1.f - hit.occluded * invSamples * aoPower) * (1.f - hit.transparency) + hit.specular;

		// Add reflections
		if (hit.material->reflectIntensity > 0.f && doReflections)
			result[i] += reflectResult[i] * hit.material->reflectIntensity;

		// Add reflections
		if (hit.transparency > 0.f && doRefractions)
			result[i] += refractResult[i] * hit.transparency;

		result[i].a(1.f);

	}
	
}