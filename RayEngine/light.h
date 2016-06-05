#pragma once

#include "vec3.h"
#include "color.h"

struct Light {

	Vec3 position;
	Color color;
	float range;

	Light(Vec3 position, Color color, float range);

};
