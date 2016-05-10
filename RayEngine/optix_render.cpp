#include "rayengine.h"

void RayEngine::optixRender() {

	if (!OPTIX_ENABLE || Optix.width == 0)
		return;

	float start = glfwGetTime();

	try {

		Optix.context["org"]->setFloat(rayOrg.x(), rayOrg.y(), rayOrg.z());
		Optix.context["xaxis"]->setFloat(rayXaxis.x(), rayXaxis.y(), rayXaxis.z());
		Optix.context["yaxis"]->setFloat(rayYaxis.x(), rayYaxis.y(), rayYaxis.z());
		Optix.context["zaxis"]->setFloat(rayZaxis.x(), rayZaxis.y(), rayZaxis.z());
		Optix.context["offset"]->setFloat(Optix.offset);
		Optix.context["windowWidth"]->setFloat(window.width);
		Optix.context["maxReflections"]->setInt(maxReflections);
		Optix.context->launch(0, Optix.width, window.height);

	} catch (optix::Exception e) {

		cout << "OptiX error: " << e.getErrorString() << endl;
		system("pause");
	}

	float end = glfwGetTime();
	Optix.lastTime = end - start;
	Optix.time += Optix.lastTime;
	Optix.frames++;
	if (Optix.frames > 30) {
		Optix.avgTime = Optix.time / Optix.frames;
		Optix.time = 0.f;
		Optix.frames = 0;
	}

}

void RayEngine::optixRenderUpdateTexture() {

	if (!OPTIX_ENABLE)
		return;

	glBindTexture(GL_TEXTURE_2D, Optix.texture);
#if OPTIX_USE_OUTPUT_VBO
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, Optix.vbo);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Optix.width, window.height, GL_RGBA, GL_FLOAT, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#else
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Optix.width, window.height, GL_RGBA, GL_FLOAT, Optix.buffer->map());
	Optix.buffer->unmap();
#endif
	glBindTexture(GL_TEXTURE_2D, 0);

	OpenGL.shdrTexture->render2DBox(window.ortho, Optix.offset, 0, Optix.width, window.height, Optix.texture);

}