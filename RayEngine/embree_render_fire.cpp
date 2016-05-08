#include "rayengine.h"

void RayEngine::embreeRenderFirePrimaryRay(int x, int y) {

	float dx = ((float)(EmbreeData.offset + x) / window.width) * 2.f - 1.f;
	float dy = ((float)y / window.height) * 2.f - 1.f;

	Vec3 rayDir = dx * rayXaxis + dy * rayYaxis + rayZaxis;

	RTCRay ray;
	ray.org[0] = rayOrg.x();
	ray.org[1] = rayOrg.y();
	ray.org[2] = rayOrg.z();
	ray.dir[0] = rayDir.x();
	ray.dir[1] = rayDir.y();
	ray.dir[2] = rayDir.z();
	ray.tnear = 0.1f;
	ray.tfar = FLT_MAX;
	ray.instID =
	ray.geomID =
	ray.primID = RTC_INVALID_GEOMETRY_ID;
	ray.mask = EMBREE_RAY_VALID;
	ray.time = 0.f;

	rtcIntersect(curScene->EmbreeData.scene, ray);

	Color result;
	embreeRenderTraceRay(ray, 0, result);

	EmbreeData.buffer[y * EmbreeData.width + x] = result;

}

void RayEngine::embreeRenderFirePrimaryPacket(int x, int y) {

	RTCRay8 packet;
	int valid[EMBREE_PACKET_SIZE];

	for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

		if (x + i >= EmbreeData.width) {
			valid[i] = EMBREE_RAY_INVALID;
			continue;
		} else
			valid[i] = EMBREE_RAY_VALID;

		float dx = ((float)(EmbreeData.offset + x + i) / window.width) * 2.f - 1.f;
		float dy = ((float)y / window.height) * 2.f - 1.f;

		Vec3 rayDir = dx * rayXaxis + dy * rayYaxis + rayZaxis;

		packet.orgx[i] = rayOrg.x();
		packet.orgy[i] = rayOrg.y();
		packet.orgz[i] = rayOrg.z();
		packet.dirx[i] = rayDir.x();
		packet.diry[i] = rayDir.y();
		packet.dirz[i] = rayDir.z();
		packet.tnear[i] = 0.1f;
		packet.tfar[i] = FLT_MAX;
		packet.instID[i] =
		packet.geomID[i] =
		packet.primID[i] = RTC_INVALID_GEOMETRY_ID;
		packet.mask[i] = EMBREE_RAY_VALID;
		packet.time[i] = 0.f;

	}

	rtcIntersect8(valid, curScene->EmbreeData.scene, packet);

	#if EMBREE_PACKET_SECONDARY

	    Color result[EMBREE_PACKET_SIZE];
		embreeRenderTracePacket(packet, valid, 0, result);

		for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
			if (valid[i] == EMBREE_RAY_VALID)
				EmbreeData.buffer[y * EmbreeData.width + x + i] = result[i];


	#else

		for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

			if (valid[i] == EMBREE_RAY_INVALID)
				continue;

			RTCRay ray;
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
			ray.mask = packet.mask[i];
			ray.time = packet.time[i];

			Color result;
			embreeRenderTraceRay(ray, 0, result);

			EmbreeData.buffer[y * EmbreeData.width + x + i] = result;

		}

	#endif

}