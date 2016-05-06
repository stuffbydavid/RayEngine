#include "rayengine.h"
/*
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
		shadowRay.mask = EMBREE_RAY_VALID;
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
	if (depth < MAX_REFLECTIONS) {

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
		rRay.mask = EMBREE_RAY_VALID;
		rRay.time = 0;

		rtcIntersect(curScene->EmbreeData.scene, rRay);

		totalReflect = embreeRenderProcessRay(rRay, depth + 1) * Color(0.25f);

	}

	// Return final color
	Color texColor = hitMaterial->diffuse * hitMaterial->image->getPixel(hitTexCoord);
	return texColor * (curScene->ambient + hitMaterial->ambient + totalDiffuse) + totalSpecular + totalReflect;

}*/