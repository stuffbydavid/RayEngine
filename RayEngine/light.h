#pragma once

#include "common.h"

struct Light {

	Vec3 position;
	Color color;
	float range;

	Light(Vec3 position, Color color, float range);

};
