#include "rayengine.h"

#define RAY_VALID -1
#define RAY_INVALID 0

void RayEngine::renderEmbree() {

	if (EmbreeData.width == 0)
		return;

	#if EMBREE_PRINT_TIME
		float start = glfwGetTime();
		printf("Frame start\n");
	#endif

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST); // Worth a shot

	Vec3 rpos = curCamera->position;
	Vec3 rxaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
	Vec3 ryaxis = curCamera->yaxis * curCamera->tFov;
	Vec3 rzaxis = curCamera->zaxis;

	#if EMBREE_RAY_LISTS

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

			renderEmbreeProcessRays(primaryRays, 0);

		}

    #else

		int numPixels = EmbreeData.width * window.height;
		int numPackets = ceil((float)numPixels / 8);

		#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < numPackets; i++) {

			RTCRay8 packet;
			__aligned(32) int valid[8];

			for (int j = 0; j < 8; j++) {

				int k = i * 8 + j;

				if (k > numPixels) {
					valid[j] = RAY_INVALID;
					continue;
				} else
					valid[j] = RAY_VALID;

				int x = k % EmbreeData.width;
				int y = k / EmbreeData.width;
				float dx = ((float)(EmbreeData.offset + x) / window.width) * 2.f - 1.f;
				float dy = ((float)y / window.height) * 2.f - 1.f;
				Vec3 rdir = dx * rxaxis + dy * ryaxis + rzaxis;

				packet.orgx[j] = rpos.x();
				packet.orgy[j] = rpos.y();
				packet.orgz[j] = rpos.z();
				packet.dirx[j] = rdir.x();
				packet.diry[j] = rdir.y();
				packet.dirz[j] = rdir.z();
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

				RTCRay ray;
				ray.org[0] = packet.orgx[j];
				ray.org[1] = packet.orgy[j];
				ray.org[2] = packet.orgz[j];
				ray.dir[0] = packet.dirx[j];
				ray.dir[1] = packet.diry[j];
				ray.dir[2] = packet.dirz[j];
				ray.tnear = packet.tnear[j];
				ray.tfar = packet.tfar[j];
				ray.instID = packet.instID[j];
				ray.geomID = packet.geomID[j];
				ray.primID = packet.primID[j];
				ray.mask = packet.mask[j];
				ray.time = packet.time[j];
				ray.u = packet.u[j];
				ray.v = packet.v[j];

				EmbreeData.buffer[k] = renderEmbreeProcessRay(ray, 0);

			}

		}

    #endif
	
	#if EMBREE_PRINT_TIME
		float end = glfwGetTime();
		printf("  Total:  %.6fs\n", end - start);
	#endif

}

#if EMBREE_RAY_LISTS

void RayEngine::renderEmbreeProcessRays(vector<EmbreeData::Ray>& rays, int depth) {

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
				EmbreeData.buffer[ray.y * EmbreeData.width + ray.x] += ray.factor * renderEmbreeSky(ray.dir);
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
		renderEmbreeProcessRays(reflectRays, depth + 1);
	}

}
#else

Color RayEngine::renderEmbreeProcessRay(RTCRay& ray, int depth) {

	if (ray.geomID == RTC_INVALID_GEOMETRY_ID)
		return renderEmbreeSky(ray.dir);

	// Store hit objects
	Object* hitObject = curScene->EmbreeData.instIDmap[ray.instID];
	TriangleMesh* hitMesh = (TriangleMesh*)hitObject->EmbreeData.geomIDmap[ray.geomID];
	Material* hitMaterial = hitMesh->material;

	// Store hit properties
	Vec3 hitPos = Vec3(ray.org) + Vec3(ray.dir) * ray.tfar;
	Vec3 hitNormal = Vec3::normalize(hitObject->matrix * hitMesh->getNormal(ray.primID, ray.u, ray.v));
	Vec2 hitTexCoord = hitMesh->getTexCoord(ray.primID, ray.u, ray.v);
	Vec3 toEye = Vec3::normalize(curCamera->position - hitPos);

	// Init colors
	Color totalDiffuse, totalSpecular, totalReflect;
	totalDiffuse = totalSpecular = totalReflect = { 0.f };

	// Add light contribution
	for (Light& light : curScene->lights) {

		float distance = Vec3::length(light.position - hitPos);
		float attenuation = max(1.f - distance / light.range, 0.f);

		// Light is too far away
		if (attenuation == 0.f)
			continue;

		// Cast shadow ray
		Vec3 incidence = Vec3::normalize(light.position - hitPos);
		RTCRay shadowRay;
		shadowRay.org[0] = hitPos.x();
		shadowRay.org[1] = hitPos.y();
		shadowRay.org[2] = hitPos.z();
		shadowRay.dir[0] = incidence.x();
		shadowRay.dir[1] = incidence.y();
		shadowRay.dir[2] = incidence.z();
		shadowRay.tnear = 0.1f;
		shadowRay.tfar = distance;
		shadowRay.instID = shadowRay.geomID = shadowRay.primID = RTC_INVALID_GEOMETRY_ID;
		shadowRay.mask = RAY_VALID;
		shadowRay.time = 0.f;

		rtcIntersect(curScene->EmbreeData.scene, shadowRay);

		// Add light to our final color if there's nothing blocking it
		if (shadowRay.geomID == RTC_INVALID_GEOMETRY_ID) {

			// Diffuse factor
			float diffuseFactor = max(Vec3::dot(hitNormal, incidence), 0.f) * attenuation;
			totalDiffuse += diffuseFactor * light.color;

			// Specular factor
			if (hitMaterial->shininess > 0.0) {
				Vec3 reflection = Vec3::reflect(incidence, hitNormal);
				float specularFactor = pow(max(Vec3::dot(reflection, toEye), 0.f), hitMaterial->shininess) * attenuation;
				totalSpecular += specularFactor * hitMaterial->specular;
			}

		}

	}

	// Reflection
	if (depth < 1) {

		Vec3 reflectDir = -Vec3::reflect(Vec3(ray.dir), hitNormal); // Embree's reflect is flipped
		RTCRay rRay;
		rRay.org[0] = hitPos.x();
		rRay.org[1] = hitPos.y();
		rRay.org[2] = hitPos.z();
		rRay.dir[0] = reflectDir.x();
		rRay.dir[1] = reflectDir.y();
		rRay.dir[2] = reflectDir.z();
		rRay.tnear = 0.1f;
		rRay.tfar = FLT_MAX;
		rRay.instID = rRay.geomID = rRay.primID = RTC_INVALID_GEOMETRY_ID;
		rRay.mask = RAY_VALID;
		rRay.time = 0;

		rtcIntersect(curScene->EmbreeData.scene, rRay);

		totalReflect = renderEmbreeProcessRay(rRay, depth + 1) * Color(0.25f);

	}

	// Return final color
	Color texColor = hitMaterial->diffuse * hitMaterial->image->getPixel(hitTexCoord);
	return texColor * (curScene->ambient + hitMaterial->ambient + totalDiffuse) + totalSpecular + totalReflect;

}

#endif

Color RayEngine::renderEmbreeSky(Vec3 dir) {

	Vec3 nDir = Vec3::normalize(dir);
	float theta = atan2f(nDir.x(), nDir.z());
	float phi = M_PIf * 0.5f - acosf(nDir.y());
	float u = (theta + M_PIf) * (0.5f * M_1_PIf);
	float v = 0.5f * (1.0f + sin(phi));
	return curScene->sky->getPixel(Vec2(u, v));

}


void RayEngine::renderEmbreeTexture() {

	if (!showEmbreeRender)
		return;

	#if EMBREE_PRINT_TIME
		float start = glfwGetTime();
	#endif

	glBindTexture(GL_TEXTURE_2D, EmbreeData.texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, EmbreeData.width, window.height, GL_RGBA, GL_FLOAT, &EmbreeData.buffer[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	shdrTexture->use(window.ortho, EmbreeData.offset, 0, EmbreeData.width, window.height, EmbreeData.texture);
	//glDrawPixels(EmbreeData.width, window.height, GL_RGBA, GL_FLOAT, &EmbreeData.buffer[0]);

	#if EMBREE_PRINT_TIME
		float end = glfwGetTime();
		printf("Embree texture: %.6fs\n", end - start);
	#endif

}