#pragma once
#include "math/affinespace.h"
#include "vec3.h"

// 4x3 Matrix
// Used for object transformations
struct Mat4x3 {

	embree::AffineSpace3fa eMat;

	// Constructors
	Mat4x3() {
		eMat = embree::AffineSpace3fa();
	}
	Mat4x3(float x1, float y1, float z1, float w1,
			float x2, float y2, float z2, float w2,
			float x3, float y3, float z3, float w3) {
		eMat = embree::AffineSpace3fa({ x1, x2, x3 }, { y1, y2, y3 }, { z1, z2, z3 }, { w1, w2, w3 });
	}
	__forceinline Mat4x3(const Mat4x3& other) {
		eMat = other.eMat;
	}
	__forceinline Mat4x3& operator=(const Mat4x3& other) {
		eMat = other.eMat;
		return *this;
	}
	Mat4x3(const embree::AffineSpace3fa& m) {
		eMat = m;
	}
	Mat4x3(const Vec3& l1, const Vec3& l2, const Vec3& l3, const Vec3& pos) {
		eMat = embree::AffineSpace3fa(l1.eVec, l2.eVec, l3.eVec, pos.eVec);
	}

	// Functions
	static __forceinline Mat4x3 translate(const Vec3& v) {
		return embree::AffineSpace3fa::translate(v.eVec);
	}
	static __forceinline Mat4x3 scale(const Vec3& s) {
		return embree::AffineSpace3fa::scale(s.eVec);
	}
	static __forceinline Vec3 rotate(const Vec3& vec, const Vec3& around, float angle) {
		return embree::xfmVector(embree::AffineSpace3fa::rotate(around.eVec, embree::deg2rad(angle)), vec.eVec);
	}

};

// Unary operators
__forceinline std::ostream& operator<<(std::ostream& cout, const Mat4x3& a) {
	return embree::operator<<(cout, a.eMat);
}

// Binary operators
__forceinline Mat4x3 operator*(const float& a, const Mat4x3& b) {
	return embree::operator*(a, b.eMat);
}
__forceinline Vec3 operator*(const Mat4x3& a, const Vec3& b) {
	return embree::xfmPoint(a.eMat, b.eVec);
}
__forceinline void operator*=(Vec3& a, const Mat4x3& b) {
	a = b * a;
}
__forceinline Mat4x3 operator*(const Mat4x3& a, const Mat4x3& b) {
	return embree::operator*(a.eMat, b.eMat);
}
__forceinline void operator*=(Mat4x3& a, const Mat4x3& b) {
	embree::operator*=(a.eMat, b.eMat);
}
	