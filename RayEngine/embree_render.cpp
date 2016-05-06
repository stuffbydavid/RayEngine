#include "rayengine.h"

void RayEngine::embreeRender() {

	if (EmbreeData.width == 0)
		return;

	#if EMBREE_PRINT_TIME
		float start = glfwGetTime();
		printf("Frame start\n");
	#endif

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST); // Worth a shot
	
    #if EMBREE_RENDER_TILES
	    embreeRenderTiles();
    #else
	    embreeRenderSingleLoop();
    #endif
	
	#if EMBREE_PRINT_TIME
		float end = glfwGetTime();
		printf("  Total:  %.6fs\n", end - start);
	#endif

}

void RayEngine::embreeRenderTiles() {

	int numTilesX = ceil((float)EmbreeData.width / EMBREE_TILE_WIDTH);
	int numTilesY = ceil((float)window.height / EMBREE_TILE_HEIGHT);
	int numTiles = numTilesX * numTilesY;

    #pragma omp parallel for schedule(dynamic, 4)
	for (int t = 0; t < numTiles; t++) {

		int tileX = t % numTilesX;
		int tileY = t / numTilesX;

		int x0 = tileX * EMBREE_TILE_WIDTH;
		int x1 = min(x0 + EMBREE_TILE_WIDTH, EmbreeData.width);
		int y0 = tileY * EMBREE_TILE_HEIGHT;
		int y1 = min(y0 + EMBREE_TILE_HEIGHT, window.height);

		for (int y = y0; y < y1; y++) {
			for (int x = x0; x < x1; x += EMBREE_PACKET_SIZE) {

                #if EMBREE_RENDER_PACKETS

					EmbreeData::RayPacket packet;

					for (int i = 0; i < EMBREE_PACKET_SIZE; i++) {

						EmbreeData.buffer[y * EmbreeData.width + x + i] = { 0.f };

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

                #else

				// TODO

                #endif
			}
		}

	}

}

void RayEngine::embreeRenderSingleLoop() {

    #if EMBREE_RENDER_PACKETS

		int numPixels = EmbreeData.width * window.height;
		int numPackets = ceil((float)numPixels / 8);

		#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < numPackets; i++) {

			EmbreeData::RayPacket packet;

			for (int j = 0; j < EMBREE_PACKET_SIZE; j++) {

				int k = i * EMBREE_PACKET_SIZE + j;

				if (k > numPixels) {
					packet.valid[j] = EMBREE_RAY_INVALID;
					continue;
				} else
					packet.valid[j] = EMBREE_RAY_VALID;

				int x = k % EmbreeData.width;
				int y = k / EmbreeData.width;
				float dx = ((float)(EmbreeData.offset + x) / window.width) * 2.f - 1.f;
				float dy = ((float)y / window.height) * 2.f - 1.f;
				Vec3 rdir = dx * rayXaxis + dy * rayYaxis + rayZaxis;

				packet.ePacket.orgx[j] = rayOrg.x();
				packet.ePacket.orgy[j] = rayOrg.y();
				packet.ePacket.orgz[j] = rayOrg.z();
				packet.ePacket.dirx[j] = rdir.x();
				packet.ePacket.diry[j] = rdir.y();
				packet.ePacket.dirz[j] = rdir.z();
				packet.ePacket.tnear[j] = 0.1f;
				packet.ePacket.tfar[j] = FLT_MAX;
				packet.ePacket.instID[j] =
				packet.ePacket.geomID[j] =
				packet.ePacket.primID[j] = RTC_INVALID_GEOMETRY_ID;
				packet.ePacket.mask[j] = EMBREE_RAY_VALID;
				packet.ePacket.time[j] = 0.f;

			}

			embreeRenderTracePacket(packet, 0);

		}

    #else

	    #pragma omp parallel num_threads(4)
		{

			// Give each thread a subsection to work with
			int wid, off;
			wid = EmbreeData.width / omp_get_num_threads();
			off = wid * omp_get_thread_num();
			if (omp_get_thread_num() == omp_get_num_threads() - 1)
				wid = EmbreeData.width - off;

			vector<EmbreeData::Ray> primaryRays(wid * window.height);

			for (int x = 0; x < wid; x++) {
				for (int y = 0; y < window.height; y++) {

					float dx = ((float)(EmbreeData.offset + off + x) / window.width) * 2.f - 1.f;
					float dy = ((float)y / window.height) * 2.f - 1.f;

					EmbreeData::Ray ray;
					ray.x = off + x;
					ray.y = y;
					ray.org = rayOrg;
					ray.dir = dx * rayXaxis + dy * rayYaxis + rayZaxis;
					ray.factor = 1.f;

					primaryRays[y * wid + x] = ray;
					EmbreeData.buffer[y * EmbreeData.width + off + x] = { 0.f };

				}
			}

			embreeRenderTraceList(primaryRays, 0);

		}

    #endif

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