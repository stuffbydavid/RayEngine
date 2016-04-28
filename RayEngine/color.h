#pragma once

#include "math/color.h"

// RGBA color
struct Color {

	embree::Color4 eCol;

	// Constructors
	Color() {
		eCol = embree::Color4();
	}
	Color(float r, float g, float b, float a = 1.f) {
		eCol = embree::Color4(r, g, b, a);
	}
	Color(embree::Color4 color) {
		eCol = color;
	}
	Color(embree::Color color) {
		eCol = embree::Color4(color);
	}
	Color(float arr[3]) {
		eCol = embree::Color4(arr[0], arr[1], arr[2], 1.f);
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
	__forceinline void r(float r) {
		eCol.r = r;
	}
	__forceinline void g(float g) {
		eCol.g = g;
	}
	__forceinline void b(float b) {
		eCol.b = b;
	}
	__forceinline void a(float a) {
		eCol.a = a;
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