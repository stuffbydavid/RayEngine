#include "rayengine.h"

void RayEngine::embreeRender() {

	if (renderMode == RM_HYBRID && !Hybrid.embree)
		return;

	Embree.renderTimer.start();

	if (Embree.width > 0) {

		fill(std::begin(Embree.buffer), std::end(Embree.buffer), 0.f);

		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST); // TODO: Find out if this does anything
	
		if (Embree.renderTiles) {

			int numTilesX = ceil((float)Embree.width / Embree.tileWidth);
			int numTilesY = ceil((float)window.height / Embree.tileHeight);
			int numTiles = numTilesX * numTilesY;

			#pragma omp parallel for schedule(dynamic)
			for (int t = 0; t < numTiles; t++) {

				int tileX = t % numTilesX;
				int tileY = t / numTilesX;

				int x0 = tileX * Embree.tileWidth;
				int x1 = min(x0 + Embree.tileWidth, Embree.width);
				int y0 = tileY * Embree.tileHeight;
				int y1 = min(y0 + Embree.tileHeight, window.height);

				if (Embree.packetPrimary) {

					for (int y = y0; y < y1; y++)
						for (int x = x0; x < x1; x += EMBREE_PACKET_SIZE)
							embreeRenderFirePrimaryPacket(x, y);

				} else {

					for (int y = y0; y < y1; y++)
						for (int x = x0; x < x1; x++)
							embreeRenderFirePrimaryRay(x, y);

				}

			}

		} else {

			if (Embree.packetPrimary) {

				#pragma omp parallel for schedule(dynamic)
				for (int y = 0; y < window.height; y++)
					for (int x = 0; x < Embree.width; x += EMBREE_PACKET_SIZE)
						embreeRenderFirePrimaryPacket(x, y);

			} else {

				#pragma omp parallel for schedule(dynamic)
				for (int y = 0; y < window.height; y++)
					for (int x = 0; x < Embree.width; x++)
						embreeRenderFirePrimaryRay(x, y);

			}

		}

	}

	Embree.renderTimer.stop();

}

void RayEngine::embreeRenderUpdateTexture() {

	if (renderMode == RM_HYBRID && !Hybrid.embree)
		return;

	Embree.textureTimer.start();

	glBindTexture(GL_TEXTURE_2D, Embree.texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, window.width, window.height, GL_RGBA, GL_FLOAT, &Embree.buffer[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	//glDrawPixels(Embree.width, window.height, GL_RGBA, GL_FLOAT, &Embree.buffer[0]);
	OpenGL.shdrTexture->render2DBox(window.ortho, Embree.offset, 0, window.width, window.height, Embree.texture, (renderMode == RM_HYBRID && Hybrid.displayPartition) ? EMBREE_HIGHLIGHT_COLOR : Color(1.f));

	Embree.textureTimer.stop();

}