#pragma once

#include "common.h"

struct OptixHandler {
	optix::Context context;
	void init();
};