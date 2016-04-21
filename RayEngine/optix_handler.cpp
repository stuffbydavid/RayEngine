#include "optix_handler.h"

void OptixHandler::init() {
	context = optix::Context::create();
	context->setRayTypeCount(1);
	context->setEntryPointCount(1);
}