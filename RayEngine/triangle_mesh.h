#pragma once

#include "common.h"
#include "material.h"
#include "geometry.h"

struct TrianglePrimitive {

	uint indices[3];

	// Constructors
	TrianglePrimitive() {}
	TrianglePrimitive(uint i0, uint i1, uint i2) {
		indices[0] = i0;
		indices[1] = i1;
		indices[2] = i2;
	}

};

struct TriangleMesh : Geometry {

	vector<Vec3> posData;
	vector<Vec3> normalData;
	vector<Vec2> texCoordData;
	vector<TrianglePrimitive> primitives;
	GLuint vbo, ibo;

	// Returns an interpolated normal
	Vec3 getNormal(int primID, float u, float v);

	// Returns an interpolated texture coordinate
	Vec2 getTexCoord(int primID, float u, float v);

	// Embree
	void initEmbree(RTCScene scene);

	// OptiX
	void initOptix(optix::Context context);
};
