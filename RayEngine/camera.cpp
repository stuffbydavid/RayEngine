#include "camera.h"

Camera::Camera() {
	xaxis = { 1, 0, 0 };
	yaxis = { 0, 1, 0 };
	zaxis = { 0, 0, -1 };
	position = { 0, 10, 50 };
	fov = 90.f;
}

Mat4x4 Camera::getMatrix(float ratio) {

	Mat4x4 P = Mat4x4::perspective(fov, ratio, 0.1f, 10000.f);
	Mat4x4 V = Mat4x4::view(xaxis, yaxis, zaxis) * Mat4x3::translate(-position);

	return P * V;

}