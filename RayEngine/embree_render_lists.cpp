#include "rayengine.h"

void RayEngine::embreeRenderLists() {

	Vec3 rpos = curCamera->position;
	Vec3 rxaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
	Vec3 ryaxis = curCamera->yaxis * curCamera->tFov;
	Vec3 rzaxis = curCamera->zaxis;

    #pragma omp parallel num_threads(4)
	{

		// Give each thread a subsection to work with
		int wid, off;
		wid = EmbreeData.width / omp_get_num_threads();
		off = wid * omp_get_thread_num();
		if (omp_get_thread_num() == omp_get_num_threads() - 1)
			wid = EmbreeData.width - off;

		// Cast primary rays
		vector<EmbreeData::Ray> primaryRays(wid * window.height);

		for (int x = 0; x < wid; x++) {
			for (int y = 0; y < window.height; y++) {

				float dx = ((float)(EmbreeData.offset + off + x) / window.width) * 2.f - 1.f;
				float dy = ((float)y / window.height) * 2.f - 1.f;

				EmbreeData::Ray ray;
				ray.x = off + x;
				ray.y = y;
				ray.pos = curScene->camera.position;
				ray.dir = dx * rxaxis + dy * ryaxis + rzaxis;
				ray.factor = 1.f;

				primaryRays[y * wid + x] = ray;
				EmbreeData.buffer[y * EmbreeData.width + off + x] = { 0.f };

			}
		}

		embreeRenderProcessList(primaryRays, 0);

	}

}


void RayEngine::embreeRenderProcessList(vector<EmbreeData::Ray>& rays, int depth) {

	// Create lists (hits/shadows/reflections)
	int numRays = rays.size();
	int numPackets = ceil((float)numRays / 8);

	vector<EmbreeData::RayHit> rayHits(numRays);
	vector<EmbreeData::ShadowRay> shadowRays(numRays * curScene->lights.size());
	vector<EmbreeData::Ray> reflectRays(numRays);

	int numRayHits, numShadowRays, numReflectRays;
	numRayHits = numShadowRays = numReflectRays = 0;

	//// Find hits ////

	for (int i = 0; i < numPackets; i++) {

		RTCRay8 packet;
		__aligned(32) int valid[8];

		for (int j = 0; j < 8; j++) {

			int k = i * 8 + j;

			if (k > numRays) {
				valid[j] = RAY_INVALID;
				continue;
			} else
				valid[j] = RAY_VALID;

			EmbreeData::Ray& ray = rays[k];

			packet.orgx[j] = ray.pos.x();
			packet.orgy[j] = ray.pos.y();
			packet.orgz[j] = ray.pos.z();
			packet.dirx[j] = ray.dir.x();
			packet.diry[j] = ray.dir.y();
			packet.dirz[j] = ray.dir.z();
			packet.tnear[j] = 0.1f;
			packet.tfar[j] = FLT_MAX;
			packet.instID[j] = packet.geomID[j] = packet.primID[j] = RTC_INVALID_GEOMETRY_ID;
			packet.mask[j] = RAY_VALID;
			packet.time[j] = 0.f;

		}

		rtcIntersect8(valid, curScene->EmbreeData.scene, packet);

		for (int j = 0; j < 8; j++) {

			if (valid[j] == RAY_INVALID)
				continue;

			int k = i * 8 + j;
			EmbreeData::Ray& ray = rays[k];

			// No hit, add background
			if (packet.geomID[j] == RTC_INVALID_GEOMETRY_ID) {
				EmbreeData.buffer[ray.y * EmbreeData.width + ray.x] += ray.factor * embreeRenderSky(ray.dir);
				continue;
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

				shadowRays[numShadowRays++] = sRay; // TODO?: Split into several lists for coherency

			}

			// Create reflection rays
			if (depth < 1) {

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
				valid[j] = RAY_INVALID;
				continue;
			} else
				valid[j] = RAY_VALID;

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
			packet.mask[j] = RAY_VALID;
			packet.time[j] = 0.f;

		}

		rtcOccluded8(valid, curScene->EmbreeData.scene, packet);

		for (int j = 0; j < 8; j++) {

			if (valid[j] == RAY_INVALID || packet.geomID[j] != RTC_INVALID_GEOMETRY_ID)
				continue;

			int k = i * 8 + j;
			EmbreeData::ShadowRay& ray = shadowRays[k];
			EmbreeData::RayHit& hit = rayHits[ray.hitID];

			// Diffuse factor
			float diffuseFactor = max(Vec3::dot(hit.normal, ray.incidence), 0.f) * ray.attenuation;
			hit.diffuse += diffuseFactor * ray.lightColor;

			// Specular factor
			if (hit.material->shininess > 0.0) {
				Vec3 toEye = Vec3::normalize(curCamera->position - hit.pos);
				Vec3 reflection = Vec3::reflect(ray.incidence, hit.normal);
				float specularFactor = pow(max(Vec3::dot(reflection, toEye), 0.f), hit.material->shininess) * ray.attenuation;
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
		embreeRenderProcessList(reflectRays, depth + 1);
	}

}