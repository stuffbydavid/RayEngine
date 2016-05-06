#include "rayengine.h"

void RayEngine::embreeRenderTraceList(vector<EmbreeData::Ray>& rays, int depth) {
	/*
	// Create lists (hits/shadows/reflections)
	int numRays = rays.size();
	int numPackets = ceil((float)numRays / 8);

	vector<EmbreeData::RayHit> rayHits(numRays);
	vector<EmbreeData::ShadowRay> shadowRays(numRays * curScene->lights.size());
	vector<EmbreeData::Ray> reflectRays(numRays);

	int numRayHits, numShadowRays, numReflectRays;
	numRayHits = numShadowRays = numReflectRays = 0;

	//// Intersection ////

	for (int i = 0; i < numPackets; i++) {

		EmbreeData::RayPacket packet;

		for (int j = 0; j < 8; j++) {

			int k = i * 8 + j;

			if (k > numRays) {
				packet.valid[j] = EMBREE_RAY_INVALID;
				continue;
			} else
				packet.valid[j] = EMBREE_RAY_VALID;

			EmbreeData::Ray& ray = rays[k];

			packet.ePacket.orgx[j] = ray.org.x();
			packet.ePacket.orgy[j] = ray.org.y();
			packet.ePacket.orgz[j] = ray.org.z();
			packet.ePacket.dirx[j] = ray.dir.x();
			packet.ePacket.diry[j] = ray.dir.y();
			packet.ePacket.dirz[j] = ray.dir.z();
			packet.ePacket.tnear[j] = 0.1f;
			packet.ePacket.tfar[j] = FLT_MAX;
			packet.ePacket.instID[j] =
			packet.ePacket.geomID[j] =
			packet.ePacket.primID[j] = RTC_INVALID_GEOMETRY_ID;
			packet.ePacket.mask[j] = EMBREE_RAY_VALID;
			packet.ePacket.time[j] = 0.f;

		}

		rtcIntersect8(packet.valid, curScene->EmbreeData.scene, packet.ePacket);

		for (int j = 0; j < 8; j++) {

			if (packet.valid[j] == EMBREE_RAY_INVALID)
				continue;

			int k = i * 8 + j;
			EmbreeData::Ray& ray = rays[k];

			// No hit, add background
			if (packet.ePacket.geomID[j] == RTC_INVALID_GEOMETRY_ID) {
				EmbreeData.buffer[ray.y * EmbreeData.width + ray.x] += {0.f}; // ray.factor * embreeRenderSky(ray.dir);
			} else {
				EmbreeData.buffer[ray.y * EmbreeData.width + ray.x] += {1.f};
			}

			// Store hit
			EmbreeData::RayHit hit;
			hit.x = ray.x;
			hit.y = ray.y;
			hit.diffuse = hit.specular = { 0.f };
			hit.pos = Vec3(packet.orgx[j], packet.orgy[j], packet.orgz[j]) + Vec3(packet.dirx[j], packet.diry[j], packet.dirz[j]) * packet.tfar[j];
			hit.obj = curScene->EmbreeData.instIDmap[packet.instID[j]];
			hit.mesh = (TriangleMesh*)hit.obj->EmbreeData.geomIDmap[packet.geomID[j]];
			hit.material = hit.mesh->material;
			hit.normal = Vec3::normalize(hit.obj->matrix * hit.mesh->getNormal(packet.primID[j], packet.u[j], packet.v[j]));
			hit.texCoord = hit.mesh->getTexCoord(packet.primID[j], packet.u[j], packet.v[j]);
			hit.factor = ray.factor;

			// Create shadow rays to all nearby lights
			for (uint l = 0; l < curScene->lights.size(); l++) {

				Light& light = curScene->lights[l];

				float distance = Vec3::length(light.position - hit.pos);
				float attenuation = max(1.f - distance / light.range, 0.f);

				// Too far away
				if (attenuation == 0.f)
					continue;

				// Store shadow ray
				EmbreeData::ShadowRay sRay;
				sRay.hitID = numRayHits;
				sRay.lightColor = light.color;
				sRay.incidence = Vec3::normalize(light.position - hit.pos);
				sRay.distance = distance;
				sRay.attenuation = attenuation;

				shadowRays[numShadowRays++] = sRay; // TODO: Maybe use separate lists for coherence?

			}

			// Create reflection rays
			if (depth < MAX_REFLECTIONS) {

				EmbreeData::Ray rRay;
				rRay.x = hit.x;
				rRay.y = hit.y;
				rRay.pos = hit.pos;
				rRay.dir = Vec3::reflect(-ray.dir, hit.normal);
				rRay.factor = ray.factor * 0.1f;

				reflectRays[numReflectRays++] = rRay;

			}

			rayHits[numRayHits++] = hit;

		}

	}
	
	//// Shadow (light) pass ////

	int numShadowPackets = ceil((float)numShadowRays / 8);

	for (int i = 0; i < numShadowPackets; i++) {

		RTCRay8 packet;
		__aligned(32) int valid[8];

		for (int j = 0; j < 8; j++) {

			int k = i * 8 + j;

			if (k > numShadowRays) {
				valid[j] = EMBREE_RAY_INVALID;
				continue;
			} else
				valid[j] = EMBREE_RAY_VALID;

			EmbreeData::ShadowRay& ray = shadowRays[k];
			EmbreeData::RayHit& hit = rayHits[ray.hitID];

			packet.orgx[j] = hit.pos.x();
			packet.orgy[j] = hit.pos.y();
			packet.orgz[j] = hit.pos.z();
			packet.dirx[j] = ray.incidence.x();
			packet.diry[j] = ray.incidence.y();
			packet.dirz[j] = ray.incidence.z();
			packet.tnear[j] = 0.1f;
			packet.tfar[j] = ray.distance;
			packet.instID[j] = packet.geomID[j] = packet.primID[j] = RTC_INVALID_GEOMETRY_ID;
			packet.mask[j] = EMBREE_RAY_VALID;
			packet.time[j] = 0.f;

		}

		rtcOccluded8(valid, curScene->EmbreeData.scene, packet);

		for (int j = 0; j < 8; j++) {

			if (valid[j] == EMBREE_RAY_INVALID || packet.geomID[j] != RTC_INVALID_GEOMETRY_ID)
				continue;

			int k = i * 8 + j;
			EmbreeData::ShadowRay& sRay = shadowRays[k];
			EmbreeData::RayHit& hit = rayHits[sRay.hitID];

			// Diffuse factor
			float diffuseFactor = max(Vec3::dot(hit.normal, sRay.incidence), 0.f) * sRay.attenuation;
			hit.diffuse += diffuseFactor * sRay.lightColor;

			// Specular factor
			if (hit.material->shininess > 0.0) {
				Vec3 toEye = Vec3::normalize(curCamera->position - hit.pos);
				Vec3 reflection = Vec3::reflect(sRay.incidence, hit.normal);
				float specularFactor = pow(max(Vec3::dot(reflection, toEye), 0.f), hit.material->shininess) * sRay.attenuation;
				hit.specular += specularFactor * hit.material->specular;
			}

		}

	}

	//// Process hits ////

	for (int i = 0; i < numRayHits; i++) {

		EmbreeData::RayHit& hit = rayHits[i];

		// Create color
		Color texColor = hit.material->diffuse * hit.material->image->getPixel(hit.texCoord);
		Color pixel = texColor * (curScene->ambient + hit.material->ambient + hit.diffuse) + hit.specular;

		EmbreeData.buffer[hit.y * EmbreeData.width + hit.x] += pixel * hit.factor;

	}

	// Process reflections
	if (numReflectRays > 0) {
		reflectRays.resize(numReflectRays);
		embreeRenderTraceList(reflectRays, depth + 1);
	}*/

}