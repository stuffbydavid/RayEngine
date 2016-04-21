#pragma once

#include "common.h"

struct EmbreeHandler {
	RTCDevice device;
	void init();
};