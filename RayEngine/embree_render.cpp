#include "rayengine.h"

void RayEngine::embreeRender() {

	if (EmbreeData.width == 0)
		return;

	float start = glfwGetTime();
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST); // Worth a shot
	
    #if EMBREE_RENDER_TILES
	    embreeRenderTiles();
    #else
	    embreeRenderSingleLoop();
    #endif

	float end = glfwGetTime();
	EmbreeData.time += end - start;
	EmbreeData.frames++;

}

void RayEngine::embreeRenderTiles() {

    #define EMBREE_TILE_WIDTH 16
    #define EMBREE_TILE_HEIGHT 16

	int numTilesX = ceil((float)EmbreeData.width / EMBREE_TILE_WIDTH);
	int numTilesY = ceil((float)window.height / EMBREE_TILE_HEIGHT);
	int numTiles = numTilesX * numTilesY;

    #pragma omp parallel for schedule(dynamic)
	for (int t = 0; t < numTiles; t++) {

		int tileX = t % numTilesX;
		int tileY = t / numTilesX;

		int x0 = tileX * EMBREE_TILE_WIDTH;
		int x1 = min(x0 + EMBREE_TILE_WIDTH, EmbreeData.width);
		int y0 = tileY * EMBREE_TILE_HEIGHT;
		int y1 = min(y0 + EMBREE_TILE_HEIGHT, window.height);

		for (int y = y0; y < y1; y++) {
			for (int x = x0; x < x1; x += EMBREE_PACKET_SIZE) {

				EmbreeData::RayPacket packet;

				for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

					if (x > EmbreeData.width) {
						packet.valid[i] = EMBREE_RAY_INVALID;
						continue;
					} else
						packet.valid[i] = EMBREE_RAY_VALID;

					float dx = ((float)(EmbreeData.offset + x + i) / window.width) * 2.f - 1.f;
					float dy = ((float)y / window.height) * 2.f - 1.f;

					packet.rays[i].x = x + i;
					packet.rays[i].y = y;
					packet.rays[i].org = rayOrg;
					packet.rays[i].dir = dx * rayXaxis + dy * rayYaxis + rayZaxis;
					packet.rays[i].factor = 1.f;
					packet.rays[i].result = { 0.f };

					// TODO: Make a function of this (fillPacket)
					packet.ePacket.orgx[i] = rayOrg.x();
					packet.ePacket.orgy[i] = rayOrg.y();
					packet.ePacket.orgz[i] = rayOrg.z();
					packet.ePacket.dirx[i] = packet.rays[i].dir.x();
					packet.ePacket.diry[i] = packet.rays[i].dir.y();
					packet.ePacket.dirz[i] = packet.rays[i].dir.z();
					packet.ePacket.tnear[i] = 0.1f;
					packet.ePacket.tfar[i] = FLT_MAX;
					packet.ePacket.instID[i] =
					packet.ePacket.geomID[i] =
					packet.ePacket.primID[i] = RTC_INVALID_GEOMETRY_ID;
					packet.ePacket.mask[i] = EMBREE_RAY_VALID;
					packet.ePacket.time[i] = 0.f;

				}

				embreeRenderTracePacket(packet, 0);

				for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
					EmbreeData.buffer[packet.rays[i].y * EmbreeData.width + packet.rays[i].x] = packet.rays[i].result;

			}
		}

	}

}

void RayEngine::embreeRenderSingleLoop() {
	
	int numPixels = EmbreeData.width * window.height;
	int numPackets = ceil((float)numPixels / 8);

	#pragma omp parallel for schedule(dynamic)
	for (int p = 0; p < numPackets; p++) {

		EmbreeData::RayPacket packet;

		for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

			int k = p * EMBREE_PACKET_SIZE + i;

			if (k > numPixels) {
				packet.valid[i] = EMBREE_RAY_INVALID;
				continue;
			} else
				packet.valid[i] = EMBREE_RAY_VALID;

			int x = k % EmbreeData.width;
			int y = k / EmbreeData.width;
			float dx = ((float)(EmbreeData.offset + x) / window.width) * 2.f - 1.f;
			float dy = ((float)y / window.height) * 2.f - 1.f;

			packet.rays[i].x = x;
			packet.rays[i].y = y;
			packet.rays[i].org = rayOrg;
			packet.rays[i].dir = dx * rayXaxis + dy * rayYaxis + rayZaxis;
			packet.rays[i].factor = 1.f;
			packet.rays[i].result = { 0.f };

			packet.ePacket.orgx[i] = rayOrg.x();
			packet.ePacket.orgy[i] = rayOrg.y();
			packet.ePacket.orgz[i] = rayOrg.z();
			packet.ePacket.dirx[i] = packet.rays[i].dir.x();
			packet.ePacket.diry[i] = packet.rays[i].dir.y();
			packet.ePacket.dirz[i] = packet.rays[i].dir.z();
			packet.ePacket.tnear[i] = 0.1f;
			packet.ePacket.tfar[i] = FLT_MAX;
			packet.ePacket.instID[i] =
			packet.ePacket.geomID[i] =
			packet.ePacket.primID[i] = RTC_INVALID_GEOMETRY_ID;
			packet.ePacket.mask[i] = EMBREE_RAY_VALID;
			packet.ePacket.time[i] = 0.f;

		}

		embreeRenderTracePacket(packet, 0);

		for (int i = 0; i < EMBREE_PACKET_SIZE; i++)
			EmbreeData.buffer[packet.rays[i].y * EmbreeData.width + packet.rays[i].x] = packet.rays[i].result;

	}

}

Color RayEngine::embreeRenderSky(Vec3 dir) {

	Vec3 nDir = Vec3::normalize(dir);
	float theta = atan2f(nDir.x(), nDir.z());
	float phi = M_PIf * 0.5f - acosf(nDir.y());
	float u = (theta + M_PIf) * (0.5f * M_1_PIf);
	float v = 0.5f * (1.0f + sin(phi));
	return curScene->sky->getPixel(Vec2(u, v));

}

void RayEngine::embreeRenderUpdateTexture() {

	glBindTexture(GL_TEXTURE_2D, EmbreeData.texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, EmbreeData.width, window.height, GL_RGBA, GL_FLOAT, &EmbreeData.buffer[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	//glDrawPixels(EmbreeData.width, window.height, GL_RGBA, GL_FLOAT, &EmbreeData.buffer[0]);
	shdrTexture->use(window.ortho, EmbreeData.offset, 0, EmbreeData.width, window.height, EmbreeData.texture);

}