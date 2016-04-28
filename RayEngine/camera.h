#pragma once

#include "common.h"
#include "vec3.h"

struct Camera {

	Camera();

	void setFov(float angle);
	Mat4x4 getMatrix(float ratio);

	Vec3 xaxis, yaxis, zaxis, position;
	float fov, tFov;

};
