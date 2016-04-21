#pragma once

#include "common.h"
#include "Vec3.h"

struct Light {

	Vec3 position;
	Color color;

	Light(Vec3 position_, Color color_) {
		position = position_;
		color = color_;
	}

};
