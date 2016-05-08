#include "rayengine.h"

void RayEngine::embreeRender() {

	if (EmbreeData.width == 0)
		return;

	float start = glfwGetTime();
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST); // Worth a shot
	
    #if EMBREE_RENDER_TILES

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

            #if EMBREE_PACKET_PRIMARY

				for (int y = y0; y < y1; y++)
					for (int x = x0; x < x1; x += EMBREE_PACKET_SIZE)
						embreeRenderFirePrimaryPacket(x, y);

			#else

				for (int y = y0; y < y1; y++)
					for (int x = x0; x < x1; x++)
						embreeRenderFirePrimaryRay(x, y);

			#endif

		}

    #else

        #if EMBREE_PACKET_PRIMARY

            #pragma omp parallel for schedule(dynamic)
			for (int y = 0; y < window.height; y++)
				for (int x = 0; x < EmbreeData.width; x += EMBREE_PACKET_SIZE)
					embreeRenderFirePrimaryPacket(x, y);

		#else

			#pragma omp parallel for schedule(dynamic)
			for (int y = 0; y < window.height; y++)
				for (int x = 0; x < EmbreeData.width; x++)
					embreeRenderFirePrimaryRay(x, y);

		#endif

    #endif

	float end = glfwGetTime();
	EmbreeData.time += end - start;
	EmbreeData.frames++;
	if (EmbreeData.frames > 30) {
		EmbreeData.avgTime = EmbreeData.time / EmbreeData.frames;
		EmbreeData.time = 0.f;
		EmbreeData.frames = 0;
	}

}


void RayEngine::embreeRenderUpdateTexture() {

	glBindTexture(GL_TEXTURE_2D, EmbreeData.texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, EmbreeData.width, window.height, GL_RGBA, GL_FLOAT, &EmbreeData.buffer[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	//glDrawPixels(EmbreeData.width, window.height, GL_RGBA, GL_FLOAT, &EmbreeData.buffer[0]);
	shdrTexture->use(window.ortho, EmbreeData.offset, 0, EmbreeData.width, window.height, EmbreeData.texture);

}