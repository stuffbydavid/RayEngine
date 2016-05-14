#include "rayengine.h"

void RayEngine::embreeRenderFirePrimaryRay(int x, int y) {

	float dx = ((float)(Embree.offset + x) / window.width) * 2.f - 1.f;
	float dy = ((float)y / window.height) * 2.f - 1.f;

	Vec3 rayDir = dx * rayXaxis + dy * rayYaxis + rayZaxis;

	Embree::Ray ray;
	ray.x = x;
	ray.y = y;
	ray.org[0] = rayOrg.x();
	ray.org[1] = rayOrg.y();
	ray.org[2] = rayOrg.z();
	ray.dir[0] = rayDir.x();
	ray.dir[1] = rayDir.y();
	ray.dir[2] = rayDir.z();
	ray.tnear = 0.01f;
	ray.tfar = FLT_MAX;
	ray.instID =
	ray.geomID =
	ray.primID = RTC_INVALID_GEOMETRY_ID;
	ray.mask = EMBREE_RAY_VALID;
	ray.time = 0.f;

	rtcIntersect(curScene->Embree.scene, ray);

	Color result;
	embreeRenderTraceRay(ray, 0, 0, result);

	Embree.buffer[y * Embree.width + x] = result;

}

void RayEngine::embreeRenderFirePrimaryPacket(int x, int y) {

	Embree::RayPacket packet;
	packet.x = x;
	packet.y = y;

	for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

		if (x + i >= Embree.width) {
			packet.valid[i] = EMBREE_RAY_INVALID;
			continue;
		} else
			packet.valid[i] = EMBREE_RAY_VALID;

		float dx = ((float)(Embree.offset + x + i) / window.width) * 2.f - 1.f;
		float dy = ((float)y / window.height) * 2.f - 1.f;

		Vec3 rayDir = dx * rayXaxis + dy * rayYaxis + rayZaxis;

		packet.orgx[i] = rayOrg.x();
		packet.orgy[i] = rayOrg.y();
		packet.orgz[i] = rayOrg.z();
		packet.dirx[i] = rayDir.x();
		packet.diry[i] = rayDir.y();
		packet.dirz[i] = rayDir.z();
		packet.tnear[i] = 0.01f;
		packet.tfar[i] = FLT_MAX;
		packet.instID[i] =
		packet.geomID[i] =
		packet.primID[i] = RTC_INVALID_GEOMETRY_ID;
		packet.mask[i] = EMBREE_RAY_VALID;
		packet.time[i] = 0.f;

	}

	rtcIntersect8(packet.valid, curScene->Embree.scene, packet);

	if (Embree.packetSecondary) {

	    Color result[EMBREE_PACKET_SIZE];
		embreeRenderTracePacket(packet, 0, 0, result);

		for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
			if (packet.valid[i] == EMBREE_RAY_VALID)
				Embree.buffer[y * Embree.width + x + i] = result[i];

	} else {

		for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

			if (packet.valid[i] == EMBREE_RAY_INVALID)
				continue;

			Embree::Ray ray;
			ray.x = x;
			ray.y = y;
			ray.org[0] = packet.orgx[i];
			ray.org[1] = packet.orgy[i];
			ray.org[2] = packet.orgz[i];
			ray.dir[0] = packet.dirx[i];
			ray.dir[1] = packet.diry[i];
			ray.dir[2] = packet.dirz[i];
			ray.tnear = packet.tnear[i];
			ray.tfar = packet.tfar[i];
			ray.instID = packet.instID[i];
			ray.geomID = packet.geomID[i];
			ray.primID = packet.primID[i];
			ray.u = packet.u[i];
			ray.v = packet.v[i];
			ray.mask = packet.mask[i];
			ray.time = packet.time[i];

			Color result;
			embreeRenderTraceRay(ray, 0, 0, result);

			Embree.buffer[y * Embree.width + x + i] = result;

		}

	}

}