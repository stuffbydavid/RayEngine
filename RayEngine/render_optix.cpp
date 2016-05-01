#include "rayengine.h"

void RayEngine::renderOptix() {
	
#if OPTIX_PRINT_TIME
	float start = glfwGetTime();
#endif

	try {

		Vec3 rpos = curCamera->position;
		Vec3 rxaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
		Vec3 ryaxis = curCamera->yaxis * curCamera->tFov;
		Vec3 rzaxis = curCamera->zaxis;
		OptixData.context["eye"]->setFloat(rpos.x(), rpos.y(), rpos.z());
		OptixData.context["xaxis"]->setFloat(rxaxis.x(), rxaxis.y(), rxaxis.z());
		OptixData.context["yaxis"]->setFloat(ryaxis.x(), ryaxis.y(), ryaxis.z());
		OptixData.context["zaxis"]->setFloat(rzaxis.x(), rzaxis.y(), rzaxis.z());
		OptixData.context["offset"]->setFloat(OptixData.offset);
		OptixData.context["windowWidth"]->setFloat(window.width);
		OptixData.context->launch(0, OptixData.width, window.height);

	} catch (optix::Exception e) {

		cout << "OptiX error: " << e.getErrorString() << endl;
		system("pause");
	}

#if OPTIX_PRINT_TIME
	float end = glfwGetTime();
	printf("Optix render:   %.6fs\n", end - start);
#endif

}

void RayEngine::renderOptixTexture() {

	if (!showOptixRender)
		return;

#if OPTIX_PRINT_TIME
	float start = glfwGetTime();
#endif

	glBindTexture(GL_TEXTURE_2D, OptixData.texture);
#if OPTIX_USE_OUTPUT_VBO
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, OptixData.vbo);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, OptixData.width, window.height, GL_RGBA, GL_FLOAT, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#else
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, OptixData.width, window.height, GL_RGBA, GL_FLOAT, OptixData.buffer->map());
	OptixData.buffer->unmap();
#endif
	glBindTexture(GL_TEXTURE_2D, 0);

	shdrTexture->use2D(window.ortho, OptixData.offset, 0, OptixData.width, window.height, OptixData.texture);

#if OPTIX_PRINT_TIME
	float end = glfwGetTime();
	printf("OptiX texture:  %.6fs\n", end - start);
#endif

}