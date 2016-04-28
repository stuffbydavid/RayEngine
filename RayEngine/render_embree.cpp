#include "rayengine.h"

#define RAY_VALID -1
#define RAY_INVALID 0

// Stores a diffuse hit
struct {
	int x, y;
	Color color;
} DiffuseHit;

void RayEngine::renderEmbree() {

	int wid, hei;
	wid = ceil(window.width * (rayTracingTarget == RTT_HYBRID ? hybridPartition : 1.f));
	hei = window.height;
	
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	Vec3 rpos = curCamera->position;
	Vec3 rxaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
	Vec3 ryaxis = curCamera->yaxis * curCamera->tFov;
	Vec3 rzaxis = curCamera->zaxis;

	// 1: Diffuse pass
	//   store texture, phong shading from intersect
	// 2: Shadow pass
	//   for non-missed pixels, check occlusion
	// 3: Reflection pass
	//   for reflective pixels, do reflection (using same code as step 1), add to pixel

	// Diffuse pass
	int numDiffusePixels = wid * hei;
	int numDiffusePackets = ceil((float)numDiffusePixels / 8);

    #pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < numDiffusePackets; i++) {

		RTCRay8 packet;
		__aligned(32) int valid[8];

		for (int j = 0; j < 8; j++) {

			if (i * 8 + j > numDiffusePixels) {
				valid[j] = RAY_INVALID;
				continue;
			} else
				valid[j] = RAY_VALID;

			int x = (i * 8 + j) % wid;
			int y = (i * 8 + j) / wid;

			float dx = ((float)x / window.width) * 2.f - 1.f;
			float dy = ((float)y / window.height) * 2.f - 1.f;
			Vec3 dir = dx * rxaxis + dy * ryaxis + rzaxis;

			packet.orgx[j] = rpos.x();
			packet.orgy[j] = rpos.y();
			packet.orgz[j] = rpos.z();
			packet.dirx[j] = dir.x();
			packet.diry[j] = dir.y();
			packet.dirz[j] = dir.z();
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

			int x = (i * 8 + j) % wid;
			int y = (i * 8 + j) / wid;

			if (packet.geomID[j] == RTC_INVALID_GEOMETRY_ID) {
				EmbreeData.buffer[y * window.width + x] = { 0.f, 0.f, 0.f };
				continue;
			}

			Object* hitObj = curScene->EmbreeData.instIDmap[packet.instID[j]];

			TriangleMesh* hitMesh = (TriangleMesh*)hitObj->EmbreeData.geomIDmap[packet.geomID[j]];
			Vec3 hitNorm = Vec3::normalize(hitObj->matrix * hitMesh->getNormal(packet.primID[j], packet.u[j], packet.v[j]));

			Vec3 n = hitNorm * 0.5f + 0.5f;
			EmbreeData.buffer[y * window.width + x] = { n.x(), n.y(), n.z() };
		
		}

	}
	
}

void RayEngine::renderEmbreeTexture() {

	glBindTexture(GL_TEXTURE_2D, EmbreeData.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.width, window.height, 0, GL_RGBA, GL_FLOAT, EmbreeData.buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	shdrTex->use2D(window.ortho, 0, 0, window.width, window.height, EmbreeData.texture);

}