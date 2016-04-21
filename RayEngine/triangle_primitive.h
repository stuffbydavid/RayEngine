#pragma once

#include "common.h"
#include "material.h"

struct TrianglePrimitive {

	uint id, indices[3];
	Material* material;

	// Constructors
	TrianglePrimitive() { }
	TrianglePrimitive(uint i0, uint i1, uint i2) {
		indices[0] = i0;
		indices[1] = i1;
		indices[2] = i2;
	}

};

