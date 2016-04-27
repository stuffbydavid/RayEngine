#pragma once

#include "common.h"
#include "material.h"
#include "geometry.h"

struct TrianglePrimitive {

	// Constructors
	TrianglePrimitive() {}
	TrianglePrimitive(uint i0, uint i1, uint i2) {
		indices[0] = i0;
		indices[1] = i1;
		indices[2] = i2;
	}

	// Triangle indices
	uint indices[3];

};

struct TriangleMesh : Geometry {

	// Returns an interpolated normal
	Vec3 getNormal(int primID, float u, float v);

	// Returns an interpolated texture coordinate
	Vec2 getTexCoord(int primID, float u, float v);

	// Variables
	vector<Vec3> posData;
	vector<Vec3> normalData;
	vector<Vec2> texCoordData;
	vector<TrianglePrimitive> indexData;
	GLuint vboPos, vboNormal, vboTexCoord, ibo;

	// Embree
	void initEmbree(RTCScene scene);

	// OptiX
	struct {
		optix::Geometry geometry;
		optix::GeometryInstance geometryInstance;
		optix::Buffer posBuffer, indexBuffer;
	} OptixData;
	void initOptix(optix::Context context);

};
