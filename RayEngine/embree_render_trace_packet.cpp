#include "rayengine.h"

void RayEngine::embreeRenderTracePacket(EmbreeData::RayPacket& packet, int depth) {
	
	// Create light packets (invalid by default)
	vector<EmbreeData::LightRayPacket> lightPackets(curScene->lights.size());
	for (EmbreeData::LightRayPacket p : lightPackets)
		for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
			p.valid[i] = EMBREE_RAY_INVALID;

	// Reflection packet (invalid by default)
	bool doReflections = false;
	EmbreeData::RayPacket reflectPacket;
	for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
		reflectPacket.valid[i] = EMBREE_RAY_INVALID;

	//// Intersection ////

	EmbreeData::RayHit hits[EMBREE_PACKET_SIZE];
	//rtcIntersect8(packet.valid, curScene->EmbreeData.scene, packet.ePacket);

	for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

		if (!packet.valid[i])
			continue;

		if (packet.ePacket.geomID[i] == RTC_INVALID_GEOMETRY_ID) {
			EmbreeData.buffer[packet.rays[i].y * EmbreeData.width + packet.rays[i].x] += embreeRenderSky(packet.rays[i].dir) * packet.rays[i].factor;
			packet.valid[i] = EMBREE_RAY_INVALID;
			continue;
		}

		// Store hit
		EmbreeData::RayHit& hit = hits[i];
		hit.diffuse = hit.specular = { 0.f };
		hit.pos = packet.rays[i].org + packet.rays[i].dir * packet.ePacket.tfar[i];
		hit.obj = curScene->EmbreeData.instIDmap[packet.ePacket.instID[i]];
		hit.mesh = (TriangleMesh*)hit.obj->EmbreeData.geomIDmap[packet.ePacket.geomID[i]];
		hit.material = hit.mesh->material;
		hit.normal = Vec3::normalize(hit.obj->matrix * hit.mesh->getNormal(packet.ePacket.primID[i], packet.ePacket.u[i], packet.ePacket.v[i]));
		hit.texCoord = hit.mesh->getTexCoord(packet.ePacket.primID[i], packet.ePacket.u[i], packet.ePacket.v[i]);

		// Check lights
		for (int l = 0; l < curScene->lights.size(); l++) {

			Light& light = curScene->lights[l];
			float distance = Vec3::length(light.position - hit.pos);
			float attenuation = max(1.f - distance / light.range, 0.f);

			// Light is too far away
			if (attenuation == 0.f)
				continue;

			// Define light ray
			EmbreeData::LightRay& lRay = lightPackets[l].rays[i];
			lRay.attenuation = attenuation;
			lRay.distance = distance;
			lRay.incidence = Vec3::normalize(light.position - hit.pos);
			lRay.lightColor = light.color;

			EMBREE_PACKET_TYPE& lePacket = lightPackets[l].ePacket;
			lePacket.orgx[i] = hit.pos.x();
			lePacket.orgy[i] = hit.pos.y();
			lePacket.orgz[i] = hit.pos.z();
			lePacket.dirx[i] = lRay.incidence.x();
			lePacket.diry[i] = lRay.incidence.y();
			lePacket.dirz[i] = lRay.incidence.z();
			lePacket.tnear[i] = 0.1f;
			lePacket.tfar[i] = distance;
			lePacket.instID[i] =
			lePacket.geomID[i] =
			lePacket.primID[i] = RTC_INVALID_GEOMETRY_ID;
			lePacket.mask[i] = EMBREE_RAY_VALID;
			lePacket.time[i] = 0.f;

			// Turn on for occlusion test
			lightPackets[l].valid[i] = EMBREE_RAY_VALID;

		}

		// Create reflection rays
		if (depth < MAX_REFLECTIONS) {

			EmbreeData::Ray& rRay = reflectPacket.rays[i];
			rRay.x = packet.rays[i].x;
			rRay.y = packet.rays[i].y;
			rRay.org = hit.pos;
			rRay.dir = Vec3::reflect(-packet.rays[i].dir, hit.normal);
			rRay.factor = packet.rays[i].factor * 0.25f;

			EMBREE_PACKET_TYPE& rePacket = reflectPacket.ePacket;
			rePacket.orgx[i] = hit.pos.x();
			rePacket.orgy[i] = hit.pos.y();
			rePacket.orgz[i] = hit.pos.z();
			rePacket.dirx[i] = rRay.dir.x();
			rePacket.diry[i] = rRay.dir.y();
			rePacket.dirz[i] = rRay.dir.z();
			rePacket.tnear[i] = 0.1f;
			rePacket.tfar[i] = FLT_MAX;
			rePacket.instID[i] =
			rePacket.geomID[i] =
			rePacket.primID[i] = RTC_INVALID_GEOMETRY_ID;
			rePacket.mask[i] = EMBREE_RAY_VALID;
			rePacket.time[i] = 0.f;

			// Turn on for recursive call
			reflectPacket.valid[i] = EMBREE_RAY_VALID;
			doReflections = true;
		}


	}

	//// Light pass ////

	for (int l = 0; l < curScene->lights.size(); l++) {

		EmbreeData::LightRayPacket& lPacket = lightPackets[l];
		//rtcOccluded8(lPacket.valid, curScene->EmbreeData.scene, lPacket.ePacket);

		for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

			// Light ray was invalid or hit an object
			if (lPacket.valid[i] == EMBREE_RAY_INVALID || lPacket.ePacket.geomID[i] != RTC_INVALID_GEOMETRY_ID)
				continue;

			EmbreeData::LightRay& lRay = lightPackets[l].rays[i];
			EmbreeData::RayHit& hit = hits[i];

			// Diffuse factor
			float diffuseFactor = max(Vec3::dot(hit.normal, lRay.incidence), 0.f) * lRay.attenuation;
			hit.diffuse += diffuseFactor * lRay.lightColor;

			// Specular factor
			if (hit.material->shininess > 0.0) {
				Vec3 toEye = Vec3::normalize(curCamera->position - hit.pos);
				Vec3 reflection = Vec3::reflect(lRay.incidence, hit.normal);
				float specularFactor = pow(max(Vec3::dot(reflection, toEye), 0.f), hit.material->shininess) * lRay.attenuation;
				hit.specular += specularFactor * hit.material->specular;
			}

		}
	}

	//// Reflections ////

	if (doReflections) {
		embreeRenderTracePacket(reflectPacket, depth + 1);
	}

	//// Store hits in buffer ////

	for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

		if (packet.valid[i] == EMBREE_RAY_INVALID)
			continue;

		EmbreeData::RayHit& hit = hits[i];

		// Create color
		Color texColor = hit.material->diffuse * hit.material->image->getPixel(hit.texCoord);
		Color pixel = texColor * (curScene->ambient + hit.material->ambient + hit.diffuse) + hit.specular;

		EmbreeData.buffer[packet.rays[i].y * EmbreeData.width + packet.rays[i].x] += pixel * packet.rays[i].factor;

	}

}