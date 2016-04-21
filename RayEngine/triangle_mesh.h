#pragma once

#include "common.h"
#include "geometry.h"
#include "triangle_primitive.h"

struct TriangleMesh : Geometry {

	int id;

	vector<Vec3> posData;
	vector<Vec3> normalData;
	vector<Vec2> texCoordData;
	vector<TrianglePrimitive> primitives;
	GLuint vbo, ibo;

	GeometryType getGeometryType() {
		return GeometryType::TriangleMesh;
	}


	Vec3 getNormal(int primID, float u, float v) {
		TrianglePrimitive& prim = primitives[primID];

		return
			(1.f - u - v) * normalData[prim.indices[0]] +
			u * normalData[prim.indices[1]] +
			v * normalData[prim.indices[2]];
	}

	Vec2 getTexCoord(int primID, float u, float v) {
		TrianglePrimitive& prim = primitives[primID];

		return 
			(1.f - u - v) * texCoordData[prim.indices[0]] +
			u * texCoordData[prim.indices[1]] +
			v * texCoordData[prim.indices[2]];
	}
};
