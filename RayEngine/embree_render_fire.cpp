#include "rayengine.h"

void RayEngine::embreeRenderFirePrimaryRay(int x, int y) {

	float dx = ((float)(EmbreeData.offset + x) / window.width) * 2.f - 1.f;
	float dy = ((float)y / window.height) * 2.f - 1.f;

	EmbreeData::Ray ray;

	ray.x = x;
	ray.y = y;
	ray.org = rayOrg;
	ray.dir = dx * rayXaxis + dy * rayYaxis + rayZaxis;
	ray.factor = 1.f;
	ray.result = { 0.f };

	ray.eRay.org[0] = ray.org.x();
	ray.eRay.org[1] = ray.org.y();
	ray.eRay.org[2] = ray.org.z();
	ray.eRay.dir[0] = ray.dir.x();
	ray.eRay.dir[1] = ray.dir.y();
	ray.eRay.dir[2] = ray.dir.z();
	ray.eRay.tnear = 0.1f;
	ray.eRay.tfar = FLT_MAX;
	ray.eRay.instID =
	ray.eRay.geomID =
	ray.eRay.primID = RTC_INVALID_GEOMETRY_ID;
	ray.eRay.mask = EMBREE_RAY_VALID;
	ray.eRay.time = 0.f;

	rtcIntersect(curScene->EmbreeData.scene, ray.eRay);
	embreeRenderTraceRay(ray, 0);

	EmbreeData.buffer[ray.y * EmbreeData.width + ray.x] = ray.result;

}

void RayEngine::embreeRenderFirePrimaryPacket(int x, int y) {

	EmbreeData::RayPacket packet;

	for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

		if (x + i >= EmbreeData.width) {
			packet.valid[i] = EMBREE_RAY_INVALID;
			continue;
		} else
			packet.valid[i] = EMBREE_RAY_VALID;

		float dx = ((float)(EmbreeData.offset + x + i) / window.width) * 2.f - 1.f;
		float dy = ((float)y / window.height) * 2.f - 1.f;

		EmbreeData::Ray& ray = packet.rays[i];

		ray.x = x + i;
		ray.y = y;
		ray.org = rayOrg;
		ray.dir = dx * rayXaxis + dy * rayYaxis + rayZaxis;
		ray.factor = 1.f;
		ray.result = { 0.f };

		packet.ePacket.orgx[i] = ray.org.x();
		packet.ePacket.orgy[i] = ray.org.y();
		packet.ePacket.orgz[i] = ray.org.z();
		packet.ePacket.dirx[i] = ray.dir.x();
		packet.ePacket.diry[i] = ray.dir.y();
		packet.ePacket.dirz[i] = ray.dir.z();
		packet.ePacket.tnear[i] = 0.1f;
		packet.ePacket.tfar[i] = FLT_MAX;
		packet.ePacket.instID[i] =
		packet.ePacket.geomID[i] =
		packet.ePacket.primID[i] = RTC_INVALID_GEOMETRY_ID;
		packet.ePacket.mask[i] = EMBREE_RAY_VALID;
		packet.ePacket.time[i] = 0.f;

	}

	rtcIntersect8(packet.valid, curScene->EmbreeData.scene, packet.ePacket);

	#if EMBREE_PACKET_SECONDARY

		embreeRenderTracePacket(packet, 0);

		for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
			if (packet.valid[i] == EMBREE_RAY_VALID)
				EmbreeData.buffer[packet.rays[i].y * EmbreeData.width + packet.rays[i].x] = packet.rays[i].result;


	#else

		for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

			if (packet.valid[i] == EMBREE_RAY_INVALID)
				continue;

			EmbreeData::Ray& ray = packet.rays[i];

			ray.eRay.org[0] = packet.ePacket.orgx[i];
			ray.eRay.org[1] = packet.ePacket.orgy[i];
			ray.eRay.org[2] = packet.ePacket.orgz[i];
			ray.eRay.dir[0] = packet.ePacket.dirx[i];
			ray.eRay.dir[1] = packet.ePacket.diry[i];
			ray.eRay.dir[2] = packet.ePacket.dirz[i];
			ray.eRay.tnear = packet.ePacket.tnear[i];
			ray.eRay.tfar = packet.ePacket.tfar[i];
			ray.eRay.instID = packet.ePacket.instID[i];
			ray.eRay.geomID = packet.ePacket.geomID[i];
			ray.eRay.primID = packet.ePacket.primID[i];
			ray.eRay.mask = packet.ePacket.mask[i];
			ray.eRay.time = packet.ePacket.time[i];

			embreeRenderTraceRay(ray, 0);

			EmbreeData.buffer[ray.y * EmbreeData.width + ray.x] = ray.result;

		}

	#endif

}