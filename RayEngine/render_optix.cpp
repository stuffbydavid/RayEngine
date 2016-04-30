#include "rayengine.h"

void RayEngine::renderOptix() {
	
#if OPTIX_PRINT_TIME
	float start = glfwGetTime();
#endif

	OptixData.context["offset"]->setFloat(OptixData.offset);
	OptixData.context["windowWidth"]->setFloat(window.width);

	Vec3 rpos = curCamera->position;
	Vec3 rxaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
	Vec3 ryaxis = curCamera->yaxis * curCamera->tFov;
	Vec3 rzaxis = curCamera->zaxis;
	OptixData.context["eye"]->setFloat(rpos.x(), rpos.y(), rpos.z());
	OptixData.context["xaxis"]->setFloat(rxaxis.x(), rxaxis.y(), rxaxis.z());
	OptixData.context["yaxis"]->setFloat(ryaxis.x(), ryaxis.y(), ryaxis.z());
	OptixData.context["zaxis"]->setFloat(rzaxis.x(), rzaxis.y(), rzaxis.z());
	OptixData.context->launch(0, OptixData.width, window.height);

#if OPTIX_PRINT_TIME
	float end = glfwGetTime();
	printf("Optix render:   %.6fs\n", end - start);
#endif

}

void RayEngine::renderOptixTexture() {

	if (!showOptixRender)
		return;

	float start = glfwGetTime();
	
	glBindTexture(GL_TEXTURE_2D, OptixData.texture);
#if OPTIX_USE_OPENGL
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, OptixData.vbo);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, OptixData.width, window.height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, OptixData.width, window.height, 0, GL_RGBA, GL_FLOAT, OptixData.buffer->map());
	OptixData.buffer->unmap();
#endif
	glBindTexture(GL_TEXTURE_2D, 0);

	shdrTex->use2D(window.ortho, OptixData.offset, 0, OptixData.width, window.height, OptixData.texture);
	float end = glfwGetTime();
	printf("OptiX texture:  %.6fs\n", end - start);

}