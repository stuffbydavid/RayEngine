#include "rayengine.h"

void RayEngine::initOptix() {

	// Make context
	OptixData.context = optix::Context::create();
	OptixData.context->setRayTypeCount(1);
	OptixData.context->setEntryPointCount(1);

	// Make buffer
	OptixData.buffer = OptixData.context->createBufferFromGLBO(RT_BUFFER_OUTPUT, window.vbo);
	OptixData.buffer->setFormat(RT_FORMAT_FLOAT4);
	OptixData.buffer->setSize(window.width, window.height);
	OptixData.context["outputBuffer"]->set(OptixData.buffer);

}

void TriangleMesh::initOptix(optix::Context context) {

}