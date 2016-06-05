#pragma once

#include "mat4x4.h"
#include "vec3.h"

struct Camera {

	Camera();

	void setFov(float angle);
	Mat4x4 getMatrix(float ratio);

	Vec3 xaxis, yaxis, zaxis, position;
	float fov, tFov;

};
