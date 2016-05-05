#pragma once

// 4x4 Matrix
struct Mat4x4 {

	// Matrix elements
	float e[16];

	//// Constructors ////

	Mat4x4() {
		e[0] = 1.f; e[4] = 0.f; e[8] = 0.f; e[12] = 0.f;
		e[1] = 0.f; e[5] = 1.f; e[9] = 0.f; e[13] = 0.f;
		e[2] = 0.f; e[6] = 0.f; e[10] = 1.f; e[14] = 0.f;
		e[3] = 0.f; e[7] = 0.f; e[11] = 0.f; e[15] = 1.f;
	}

	Mat4x4(float x1, float y1, float z1, float w1,
		float x2, float y2, float z2, float w2,
		float x3, float y3, float z3, float w3,
		float x4, float y4, float z4, float w4) {
		e[0] = x1; e[4] = y1; e[8] = z1; e[12] = w1;
		e[1] = x2; e[5] = y2; e[9] = z2; e[13] = w2;
		e[2] = x3; e[6] = y3; e[10] = z3; e[14] = w3;
		e[3] = x4; e[7] = y4; e[11] = z4; e[15] = w4;
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

	//// Functions ////

	// Builds a translation matrix.
	static __forceinline Mat4x4 translate(const Vec3& vec) {
		return Mat4x4(
			1.f, 0.f, 0.f, vec.x(),
			0.f, 1.f, 0.f, vec.y(),
			0.f, 0.f, 1.f, vec.z(),
			0.f, 0.f, 0.f, 1.f
		);
	}

	// Builds a rotation matrix.
	static __forceinline Mat4x4 rotate(const Vec3& around, float angle) {
		embree::LinearSpace3f ls = embree::LinearSpace3f::rotate(around.eVec, embree::deg2rad(angle));
		return Mat4x4(
			ls.vx.x, ls.vy.x, ls.vz.x, 0.f,
			ls.vx.y, ls.vy.y, ls.vz.y, 0.f,
			ls.vx.z, ls.vy.z, ls.vz.z, 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}

	// Builds a scaling matrix.
	static __forceinline Mat4x4 scale(const Vec3& s) {
		return Mat4x4(
			s.x(), 0.f, 0.f, 0.f,
			0.f, s.y(), 0.f, 0.f,
			0.f, 0.f, s.z(), 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}

	// Builds an orthographic matrix.
	static __forceinline Mat4x4 ortho(float left, float right, float bottom, float top, float znear, float zfar) {
		return Mat4x4(
			2.f / (right - left), 0.f, 0.f, -(right + left) / (right - left),
			0.f, 2.f / (top - bottom), 0.f, -(top + bottom) / (top - bottom),
			0.f, 0.f, -2.f / (zfar - znear), -(zfar + znear) / (zfar - znear),
			0.f, 0.f, 0.f, 1.f
		);
	}

	// Builds a perspective matrix.
	static __forceinline Mat4x4 perspective(float tFov, float ratio, float znear, float zfar) {
		float iFov = 1.f / tFov;
		return Mat4x4(
			iFov / ratio, 0.f, 0.f, 0.f,
			0.f, iFov, 0.f, 0.f,
			0.f, 0.f, (zfar + znear) / (znear - zfar), (2.f * zfar * znear) / (znear - zfar),
			0.f, 0.f, -1.f, 0.f
		);
	}

	// Builds a view matrix from a x, y and z axis.
	static __forceinline Mat4x4 view(const Vec3& xaxis, const Vec3& yaxis, const Vec3& zaxis) {
		return Mat4x4(
			xaxis.x(), xaxis.y(), xaxis.z(), 0.f,
			yaxis.x(), yaxis.y(), yaxis.z(), 0.f,
			-zaxis.x(), -zaxis.y(), -zaxis.z(), 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}

};

//// Unary operators ////

__forceinline std::ostream& operator << (std::ostream& cout, const Mat4x4& a) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			cout << a.e[j * 4 + i] << (j < 3 ? "," : "");
		cout << std::endl;
	}
	return cout;
}

//// Binary operators ////

__forceinline Mat4x4 operator * (const Mat4x4& a, const Mat4x4& b) {
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

__forceinline Vec3 operator * (const Mat4x4& a, const Vec3& b) {
	return Vec3(
		a.e[0] * b.x() + a.e[4] * b.y() + a.e[8] * b.z() + a.e[12] * 0.f,
		a.e[1] * b.x() + a.e[5] * b.y() + a.e[9] * b.z() + a.e[13] * 0.f,
		a.e[2] * b.x() + a.e[6] * b.y() + a.e[10] * b.z() + a.e[14] * 0.f
	);
}

__forceinline void operator *= (Mat4x4& a, const Mat4x4& b) {
	a = a * b;
}