#include "camera.h"

Camera::Camera() {
	xaxis = { 1.f, 0.f, 0.f };
	yaxis = { 0.f, 1.f, 0.f };
	zaxis = { 0.f, 0.f, -1.f };
	position = { 0.f, 10.f, 50.f };
	setFov(90.f);
}

void Camera::setFov(float angle) {
	fov = angle;
	tFov = embree::tan(embree::deg2rad(fov / 2.f));
}

Mat4x4 Camera::getMatrix(float ratio) {
	Mat4x4 P = Mat4x4::perspective(tFov, ratio, 0.1f, 10000.f);
	Mat4x4 V = Mat4x4::view(xaxis, yaxis, zaxis) * Mat4x4::translate(-position);
	return P * V;
}