#include "rayengine.h"

void RayEngine::embreeRenderOld() {

	Vec3 rpos = curCamera->position;
	Vec3 rxaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
	Vec3 ryaxis = curCamera->yaxis * curCamera->tFov;
	Vec3 rzaxis = curCamera->zaxis;

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

			EmbreeData.buffer[k] = embreeRenderProcessRay(ray, 0);

		}

	}

}

Color RayEngine::embreeRenderProcessRay(RTCRay& ray, int depth) {

	if (ray.geomID == RTC_INVALID_GEOMETRY_ID)
		return embreeRenderSky(ray.dir);

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

		totalReflect = embreeRenderProcessRay(rRay, depth + 1) * Color(0.25f);

	}

	// Return final color
	Color texColor = hitMaterial->diffuse * hitMaterial->image->getPixel(hitTexCoord);
	return texColor * (curScene->ambient + hitMaterial->ambient + totalDiffuse) + totalSpecular + totalReflect;

}