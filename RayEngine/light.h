#pragma once

#include "common.h"

struct Light {

	Vec3 position;
	Color color;

	Light(Vec3 position, Color color) :
	  position(position),
	  color(color) {}

};
