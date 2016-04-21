#pragma once

#include "math/color.h"

// RGBA color
struct Color {

	embree::Color4 eCol;

	// Constructors
	Color() {
		eCol = embree::Color4();
	}

	Color(float r_, float g_, float b_, float a_) {
		eCol = embree::Color4(r_, g_, b_, a_);
	}
	Color(embree::Color4 color_) {
		eCol = color_;
	}
	Color(embree::Color color_) {
		eCol = embree::Color4(color_);
	}
	Color(float arr[3]) {
		eCol = embree::Color4(arr[0], arr[1], arr[2], 1);
	}

	// Getters
	__forceinline float r() const {
		return eCol.r;
	}
	__forceinline float g() const {
		return eCol.g;
	}
	__forceinline float b() const {
		return eCol.b;
	}
	__forceinline float a() const {
		return eCol.a;
	}

	// Setters
	__forceinline void r(float r_) {
		eCol.r = r_;
	}
	__forceinline void g(float g_) {
		eCol.g = g_;
	}
	__forceinline void b(float b_) {
		eCol.b = b_;
	}
	__forceinline void a(float a_) {
		eCol.a = a_;
	}

};

// Binary operators
__forceinline Color operator+(const Color& x, const Color& y) {
	return Color(embree::operator+(x.eCol, y.eCol));
}
__forceinline void operator+=(Color& x, const Color& y) {
	x = x + y;
}
__forceinline Color operator*(const Color& x, const Color& y) {
	return Color(embree::operator*(x.eCol, y.eCol));
}
__forceinline Color operator*(float x, const Color& y) {
	return Color(embree::operator*(x, y.eCol));
}
__forceinline Color operator*(const Color& x, float y) {
	return Color(embree::operator*(x.eCol, y));
}
__forceinline void operator*=(Color& x, const Color& y) {
	x = x * y;
}
__forceinline void operator*=(Color& x, float y) {
	x = x * y;
}