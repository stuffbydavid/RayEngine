#pragma once

#include "common.h"
#include "vec3.h"

struct Camera {

	Camera();

	Mat4x4 getMatrix(float ratio);

	Vec3 xaxis, yaxis, zaxis, position;
	float fov;

};
