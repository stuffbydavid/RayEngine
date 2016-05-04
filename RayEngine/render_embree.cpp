#include "rayengine.h"

#define RAY_VALID -1
#define RAY_INVALID 0

void RayEngine::renderEmbree() {

#if EMBREE_PRINT_TIME
	float start = glfwGetTime();
	printf("Frame start\n");
#endif

	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	// Create primary rays

	vector<EmbreeData::Ray> primaryRays(EmbreeData.width * window.height);
	Vec3 rxaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
	Vec3 ryaxis = curCamera->yaxis * curCamera->tFov;
	Vec3 rzaxis = curCamera->zaxis;

    #pragma omp parallel for schedule(dynamic)
	for (int x = 0; x < EmbreeData.width; x++) {
		for (int y = 0; y < window.height; y++) {

			float dx = ((float)(EmbreeData.offset + x) / window.width) * 2.f - 1.f;
			float dy = ((float)y / window.height) * 2.f - 1.f;

			EmbreeData::Ray ray;
			ray.x = x;
			ray.y = y;
			ray.pos = curScene->camera.position;
			ray.dir = dx * rxaxis + dy * ryaxis + rzaxis;
			ray.factor = 1.f;

			primaryRays[y * EmbreeData.width + x] = ray;
			EmbreeData.buffer[y * EmbreeData.width + x] = { 0.f };

		}
	}
	
	renderEmbreeProcessRays(primaryRays, 0);

#if EMBREE_PRINT_TIME
	float end = glfwGetTime();
	printf("  Total:  %.6fs\n", end - start);
#endif

}

void RayEngine::renderEmbreeProcessRays(vector<EmbreeData::Ray>& rays, int depth) {

	int numRays = rays.size();
	int numPackets = ceil((float)numRays / 8);

	vector<EmbreeData::RayHit> rayHits(numRays);
	vector<EmbreeData::ShadowRay> shadowRays(numRays * curScene->lights.size());
	vector<EmbreeData::Ray> reflectRays(numRays);

	int numRayHits, numShadowRays, numReflectRays;
	numRayHits = numShadowRays = numReflectRays = 0;

#if EMBREE_PRINT_TIME
	float start = glfwGetTime();
#endif

	// Find hits

	// TODO Try 16x16 packets
    #pragma omp parallel for schedule(dynamic)
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

        //#pragma omp parallel for schedule(dynamic)
		for (int j = 0; j < 8; j++) {

			if (valid[j] == RAY_INVALID)
				continue;

			int k = i * 8 + j;
			EmbreeData::Ray& ray = rays[k];

			if (packet.geomID[j] == RTC_INVALID_GEOMETRY_ID) {
                #pragma omp critical
				EmbreeData.buffer[ray.y * EmbreeData.width + ray.x] += ray.factor * Color(0.3f, 0.3f, 0.9f);
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

			// Create shadow rays

			for (uint l = 0; l < curScene->lights.size(); l++) {
				
				Light& light = curScene->lights[l];

				float distance = Vec3::length(light.position - hit.pos);
				float attenuation = max(1.f - distance / light.range, 0.f);

				if (attenuation == 0.f)
					continue;

				EmbreeData::ShadowRay sRay;
				sRay.hitID = numRayHits;
				sRay.lightColor = light.color;
				sRay.incidence = Vec3::normalize(light.position - hit.pos);
				sRay.distance = distance;
				sRay.attenuation = attenuation;

                #pragma omp critical
				shadowRays[numShadowRays++] = sRay; // TODO: Split into several vectors for coherency

			}

			// Create reflection rays
			if (depth < 1) {

				EmbreeData::Ray rRay;
				rRay.x = hit.x;
				rRay.y = hit.y;
				rRay.pos = hit.pos;
				rRay.dir = Vec3::reflect(-ray.dir, hit.normal);
				rRay.factor = ray.factor * 0.1f;

                #pragma omp critical
				reflectRays[numReflectRays++] = rRay;

			}

            #pragma omp critical
			rayHits[numRayHits++] = hit;
		
		}

	}

#if EMBREE_PRINT_TIME
	float end = glfwGetTime();
	printf("  Hit pass:  %.6fs\n", end - start);
	start = glfwGetTime();
#endif

	// Shadow (light) pass
	
	int numShadowPackets = ceil((float)numShadowRays / 8);

    #pragma omp parallel for schedule(dynamic)
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
				Vec3 reflection = Vec3::reflect(-ray.incidence, hit.normal);
				float specularFactor = pow(max(Vec3::dot(reflection, toEye), 0.f), hit.material->shininess) * ray.attenuation;
				hit.specular += specularFactor * hit.material->specular;
			}

		}

	}

#if EMBREE_PRINT_TIME
	end = glfwGetTime();
	printf("  Light pass:  %.6fs\n", end - start);
	start = glfwGetTime();
#endif

	// Process hits

    #pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < numRayHits; i++) {

		EmbreeData::RayHit& hit = rayHits[i];

		// Create color
		Color texColor = hit.material->diffuse * hit.material->image->getPixel(hit.texCoord);
		Color pixel = texColor * (curScene->ambient + hit.material->ambient + hit.diffuse) + hit.specular;

        #pragma omp critical
		EmbreeData.buffer[hit.y * EmbreeData.width + hit.x] += pixel * hit.factor;

	}

#if EMBREE_PRINT_TIME
	end = glfwGetTime();
	printf("  Process hits:  %.6fs\n", end - start);
	start = glfwGetTime();
#endif

	// Process reflections

	if (numReflectRays > 0) {
		reflectRays.resize(numReflectRays);
		renderEmbreeProcessRays(reflectRays, depth + 1);
	}

}

void RayEngine::renderEmbreeTexture() {

	if (!showEmbreeRender)
		return;

#if EMBREE_PRINT_TIME
	float start = glfwGetTime();
#endif

	glBindTexture(GL_TEXTURE_2D, EmbreeData.texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, EmbreeData.width, window.height, GL_RGBA, GL_FLOAT, EmbreeData.buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	shdrTexture->use(window.ortho, EmbreeData.offset, 0, EmbreeData.width, window.height, EmbreeData.texture);
	//glDrawPixels(EmbreeData.width, window.height, GL_RGBA, GL_FLOAT, EmbreeData.buffer);

#if EMBREE_PRINT_TIME
	float end = glfwGetTime();
	printf("Embree texture: %.6fs\n", end - start);
#endif

}