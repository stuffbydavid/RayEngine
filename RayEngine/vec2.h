#pragma once

#include "math/vec2.h"

// 2D vector
struct Vec2 {

	embree::Vec2f eVec;

	// Constructors
	Vec2() {
		eVec = embree::Vec2f();
	}
	Vec2(float x, float y) {
		eVec = embree::Vec2f(x, y);
	}
	Vec2(float x[2]) {
		eVec = embree::Vec2f(x[0], x[1]);
	}
	__forceinline Vec2(const Vec2& other) {
		eVec = other.eVec;
	}
	__forceinline Vec2& operator=(const Vec2& other) {
		eVec = other.eVec;
		return *this;
	}

	Vec2(embree::Vec2f v) {
		eVec = v;
	}

	// Getters
	__forceinline float x() const {
		return eVec.x;
	}
	__forceinline float y() const {
		return eVec.y;
	}

	// Setters
	__forceinline void x(float x_) {
		eVec.x = x_;
	}
	__forceinline void y(float y_) {
		eVec.y = y_;
	}

};

// Unary operators
__forceinline Vec2 operator-(const Vec2& a) {
	return embree::operator-(a.eVec);
}
__forceinline std::ostream& operator<<(std::ostream& cout, const Vec2& a) {
	return embree::operator<<(cout, a.eVec);
}

// Binary operators
__forceinline Vec2 operator+(const Vec2& a, const Vec2& b) {
	return embree::operator+(a.eVec, b.eVec);
}
__forceinline void operator+=(Vec2& a, const Vec2& b) {
	embree::operator+=(a.eVec, b.eVec);
}
__forceinline Vec2 operator-(const Vec2& a, const Vec2& b) {
	return embree::operator-(a.eVec, b.eVec);
}
__forceinline void operator-=(Vec2& a, const Vec2& b) {
	embree::operator-=(a.eVec, b.eVec);
}
__forceinline Vec2 operator*(const Vec2& a, const float& b) {
	return embree::operator*(a.eVec, b);
}
__forceinline Vec2 operator*(const float& a, const Vec2& b) {
	return embree::operator*(a, b.eVec);
}
__forceinline void operator*=(Vec2& a, const float& b) {
	embree::operator*=(a.eVec, b);
}

// Comparison operators
__forceinline bool operator==(const Vec2& a, const Vec2& b) {
	return embree::operator==(a.eVec, b.eVec);
}
__forceinline bool operator!=(const Vec2& a, const Vec2& b) {
	return embree::operator!=(a.eVec, b.eVec);
}

// Functions
__forceinline float length(const Vec2& a) {
	return embree::length(a.eVec);
}
__forceinline Vec2 normalize(const Vec2& a) {
	return embree::normalize(a.eVec);
}
__forceinline float distance(const Vec2& a, const Vec2& b) {
	return embree::distance(a.eVec, b.eVec);
}
__forceinline float dot(const Vec2& a, const Vec2& b) {
	return embree::dot(a.eVec, b.eVec);
}