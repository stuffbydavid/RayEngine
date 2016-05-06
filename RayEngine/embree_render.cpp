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
    #elif EMBREE_RENDER_LISTS
	    embreeRenderLists();
    #else
	    embreeRenderOld();
    #endif
	
	#if EMBREE_PRINT_TIME
		float end = glfwGetTime();
		printf("  Total:  %.6fs\n", end - start);
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