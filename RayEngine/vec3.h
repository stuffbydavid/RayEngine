#pragma once

#include "math/vec3.h"
#include "math/vec3fa.h"
#include "math/affinespace.h"

// 3D vector
struct Vec3 {

	embree::Vec3f eVec;

	// Constructors
	Vec3() {
		eVec = embree::Vec3f();
	}
	Vec3(float x) {
		eVec = embree::Vec3f(x, x, x);
	}
	Vec3(float x, float y, float z) {
		eVec = embree::Vec3f(x, y, z);
	}
	Vec3(int x, int y, int z) {
		eVec = embree::Vec3f(x, y, z);
	}
	Vec3(float x[3]) {
		eVec = embree::Vec3f(x[0], x[1], x[2]);
	}
	__forceinline Vec3(const Vec3& other) {
		eVec = other.eVec;
	}
	__forceinline Vec3& operator=(const Vec3& other) {
		eVec = other.eVec;
		return *this;
	}

	Vec3(embree::Vec3f v) {
		eVec = v;
	}

	// Getters
	__forceinline float x() const {
		return eVec.x;
	}
	__forceinline float y() const {
		return eVec.y;
	}
	__forceinline float z() const {
		return eVec.z;
	}

	// Setters
	__forceinline void x(float x) {
		eVec.x = x;
	}
	__forceinline void y(float y) {
		eVec.y = y;
	}
	__forceinline void z(float z) {
		eVec.z = z;
	}

	// Functions
	static __forceinline float length(const Vec3& a) {
		return embree::length(a.eVec);
	}
	static __forceinline Vec3 normalize(const Vec3& a) {
		return embree::normalize(a.eVec);
	}
	static __forceinline float distance(const Vec3& a, const Vec3& b) {
		return embree::distance(a.eVec, b.eVec);
	}
	static __forceinline float dot(const Vec3& a, const Vec3& b) {
		return embree::dot(a.eVec, b.eVec);
	}
	static __forceinline Vec3 cross(const Vec3& a, const Vec3& b) {
		return embree::cross(a.eVec, b.eVec);
	}
	static __forceinline Vec3 reflect(const Vec3& incidence, const Vec3& normal) {
		return embree::reflect(incidence.eVec, normal.eVec);
	}
	static __forceinline Vec3 rotate(const Vec3& vec, const Vec3& around, float angle) {
		return embree::xfmVector(embree::AffineSpace3fa::rotate(around.eVec, embree::deg2rad(angle)), vec.eVec);
	}

};

// Unary operators
__forceinline Vec3 operator-(const Vec3& a) {
	return embree::operator-(a.eVec);
}
__forceinline std::ostream& operator<<(std::ostream& cout, const Vec3& a) {
	return embree::operator<<(cout, a.eVec);
}

// Binary operators
__forceinline Vec3 operator+(const Vec3& a, const Vec3& b) {
	return embree::operator+(a.eVec, b.eVec);
}
__forceinline void operator+=(Vec3& a, const Vec3& b) {
	embree::operator+=(a.eVec, b.eVec);
}
__forceinline Vec3 operator-(const Vec3& a, const Vec3& b) {
	return embree::operator-(a.eVec, b.eVec);
}
__forceinline void operator-=(Vec3& a, const Vec3& b) {
	embree::operator-=(a.eVec, b.eVec);
}
__forceinline Vec3 operator*(const Vec3& a, const float& b) {
	return embree::operator*(a.eVec, b);
}
__forceinline Vec3 operator*(const float& a, const Vec3& b) {
	return embree::operator*(a, b.eVec);
}
__forceinline void operator*=(Vec3& a, const float& b) {
	embree::operator*=(a.eVec, b);
}

// Comparison operators
__forceinline bool operator==(const Vec3& a, const Vec3& b) {
	return embree::operator==(a.eVec, b.eVec);
}
__forceinline bool operator!=(const Vec3& a, const Vec3& b) {
	return embree::operator!=(a.eVec, b.eVec);
}