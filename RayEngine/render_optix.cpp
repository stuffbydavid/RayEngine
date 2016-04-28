#include "rayengine.h"

void RayEngine::renderOptix() {

	Vec3 rpos = curCamera->position;
	Vec3 rxaxis = curCamera->xaxis * window.ratio * curCamera->tFov;
	Vec3 ryaxis = curCamera->yaxis * curCamera->tFov;
	Vec3 rzaxis = curCamera->zaxis;

	OptixData.context["partition"]->setFloat(rayTracingTarget == RTT_HYBRID ? hybridPartition : 0.f);
	OptixData.context["eye"]->setFloat(rpos.x(), rpos.y(), rpos.z());
	OptixData.context["xaxis"]->setFloat(rxaxis.x(), rxaxis.y(), rxaxis.z());
	OptixData.context["yaxis"]->setFloat(ryaxis.x(), ryaxis.y(), ryaxis.z());
	OptixData.context["zaxis"]->setFloat(rzaxis.x(), rzaxis.y(), rzaxis.z());
	OptixData.context->launch(0, window.width, window.height);

}

void RayEngine::renderOptixTexture() {

	glBindTexture(GL_TEXTURE_2D, OptixData.texture);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, OptixData.vbo);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, window.width, window.height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	shdrTex->use2D(window.ortho, 0, 0, window.width, window.height, OptixData.texture);

}