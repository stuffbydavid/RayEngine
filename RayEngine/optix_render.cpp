#include "rayengine.h"

void RayEngine::optixRender() {

	if (!OPTIX_ENABLE || OptixData.width == 0)
		return;

#if OPTIX_PRINT_TIME
	float start = glfwGetTime();
#endif

	try {

		OptixData.context["org"]->setFloat(rayOrg.x(), rayOrg.y(), rayOrg.z());
		OptixData.context["xaxis"]->setFloat(rayXaxis.x(), rayXaxis.y(), rayXaxis.z());
		OptixData.context["yaxis"]->setFloat(rayYaxis.x(), rayYaxis.y(), rayYaxis.z());
		OptixData.context["zaxis"]->setFloat(rayZaxis.x(), rayZaxis.y(), rayZaxis.z());
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

void RayEngine::optixRenderUpdateTexture() {

	if (!OPTIX_ENABLE)
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

	shdrTexture->use(window.ortho, OptixData.offset, 0, OptixData.width, window.height, OptixData.texture);

#if OPTIX_PRINT_TIME
	float end = glfwGetTime();
	printf("OptiX texture:  %.6fs\n", end - start);
#endif

}