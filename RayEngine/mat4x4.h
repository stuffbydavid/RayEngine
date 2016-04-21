#pragma once

#include "mat4x3.h"

// 4x4 Matrix
// Used for viewing transformations
struct Mat4x4 {

	// Matrix elements
	float e[16];

	// Constructors
	Mat4x4() {
		e[0] = 1; e[1] = 0; e[2] = 0; e[3] = 0;
		e[4] = 0; e[5] = 1; e[6] = 0; e[7] = 0;
		e[8] = 0; e[9] = 0; e[10] = 1; e[11] = 0;
		e[12] = 0; e[13] = 0; e[14] = 0; e[15] = 1;
	}
	Mat4x4(float x1, float y1, float z1, float w1,
		float x2, float y2, float z2, float w2,
		float x3, float y3, float z3, float w3,
		float x4, float y4, float z4, float w4) {
		e[0] = x1; e[1] = x2; e[2] = x3; e[3] = x4;
		e[4] = y1; e[5] = y2; e[6] = y3; e[7] = y4;
		e[8] = z1; e[9] = z2; e[10] = z3; e[11] = z4;
		e[12] = w1; e[13] = w2; e[14] = w3; e[15] = w4;
	}
	__forceinline Mat4x4(const Mat4x4& other) {
		for (int i = 0; i < 16; i++)
			e[i] = other.e[i];
	}
	__forceinline Mat4x4& operator=(const Mat4x4& other) {
		for (int i = 0; i < 16; i++)
			e[i] = other.e[i];
		return *this;
	}
	Mat4x4(const Mat4x3& other) {
		embree::LinearSpace3fa ls = other.eMat.l;
		embree::Vec3fa as = other.eMat.p;
		e[0] = ls.vx.x; e[1] = ls.vy.x; e[2] = ls.vz.x; e[3] = as.x;
		e[4] = ls.vx.y; e[5] = ls.vy.y; e[6] = ls.vz.y; e[7] = as.y;
		e[8] = ls.vx.z; e[9] = ls.vy.z; e[10] = ls.vz.z; e[11] = as.z;
		e[12] = 0; e[13] = 0; e[14] = 0; e[15] = 1;
	}

	// Functions

	// Builds an orthographic matrix
	static __forceinline Mat4x4 ortho(float left, float right, float bottom, float top, float znear, float zfar) {
		return Mat4x4(
			2.f / (right - left), 0.f, 0.f, -(right + left) / (right - left),
			0.f, 2.f / (top - bottom), 0.f, -(top + bottom) / (top - bottom),
			0.f, 0.f, -2.f / (zfar - znear), -(zfar + znear) / (zfar - znear),
			0.f, 0.f, 0.f, 1.f
		);
	}

	// Builds a perspective matrix
	static __forceinline Mat4x4 perspective(float fov, float ratio, float znear, float zfar) {
		float f = 1.f / embree::tan(embree::deg2rad(fov) / 2.f);
		return Mat4x4(
			f / ratio, 0.f, 0.f, 0.f,
			0.f, f, 0.f, 0.f,
			0.f, 0.f, (zfar + znear) / (znear - zfar), (2.f * zfar * znear) / (znear - zfar),
			0.f, 0.f, -1.f, 0.f
		);
	}

};

// Unary operators
__forceinline std::ostream& operator<<(std::ostream& cout, const Mat4x4& a) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			cout << a.e[i * 4 +j ] << (j < 3 ? "," : "");
		cout << std::endl;
	}
	return cout;
}

// Binary operators
__forceinline Mat4x4 operator*(const Mat4x4& a, const Mat4x4& b) {
	Mat4x4 product;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			product.e[j * 4 + i] = 0;
			for (int k = 0; k < 4; k++)
				product.e[j * 4 + i] += a.e[k * 4 + i] * b.e[j * 4 + k];
		}
	}
	return product;
}
__forceinline void operator*=(Mat4x4& a, const Mat4x4& b) {
	a = a * b;
}