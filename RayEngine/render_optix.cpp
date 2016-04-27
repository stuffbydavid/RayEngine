#include "rayengine.h"

void RayEngine::renderOptix() {

	// Resize buffer object (MOVE THIS)
	OptixData.buffer->setSize(window.width, window.height);

	// Resize associated VBO
	OptixData.buffer->unregisterGLBuffer();
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, OptixData.vbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(float) * 4 * window.width * window.height, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	OptixData.buffer->registerGLBuffer();

	float ratio = (float)window.width / window.height;
	float tfov = embree::tan(embree::deg2rad(scenes[0]->camera.fov / 2.f));
	Vec3 rpos = scenes[0]->camera.position;
	Vec3 rxaxis = scenes[0]->camera.xaxis * ratio * tfov;
	Vec3 ryaxis = scenes[0]->camera.yaxis * tfov;
	Vec3 rzaxis = scenes[0]->camera.zaxis;

	OptixData.context["partition"]->setFloat(rayTracingTarget == RTT_HYBRID ? hybridPartition : 0.f);
	OptixData.context["eye"]->setFloat(rpos.x(), rpos.y(), rpos.z());
	OptixData.context["xaxis"]->setFloat(rxaxis.x(), rxaxis.y(), rxaxis.z());
	OptixData.context["yaxis"]->setFloat(ryaxis.x(), ryaxis.y(), ryaxis.z());
	OptixData.context["zaxis"]->setFloat(rzaxis.x(), rzaxis.y(), rzaxis.z());
	OptixData.context->launch(0, window.width, window.height);

	glBindTexture(GL_TEXTURE_2D, OptixData.texture);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, OptixData.vbo);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, window.width, window.height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// TODO: buffer for this?
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f);
	glVertex2f(0.f, 0.f);
	glTexCoord2f(1.f, 0.f);
	glVertex2f(1.f, 0.f);
	glTexCoord2f(1.f, 1.f);
	glVertex2f(1.f, 1.f);
	glTexCoord2f(0.f, 1.f);
	glVertex2f(0.f, 1.f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

}