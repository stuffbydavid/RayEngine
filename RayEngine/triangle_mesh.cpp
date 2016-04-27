#include "triangle_mesh.h"

Vec3 TriangleMesh::getNormal(int primID, float u, float v) {
	TrianglePrimitive& prim = primitives[primID];

	return
		(1.f - u - v) * normalData[prim.indices[0]] +
		u * normalData[prim.indices[1]] +
		v * normalData[prim.indices[2]];
}

Vec2 TriangleMesh::getTexCoord(int primID, float u, float v) {
	TrianglePrimitive& prim = primitives[primID];

	return
		(1.f - u - v) * texCoordData[prim.indices[0]] +
		u * texCoordData[prim.indices[1]] +
		v * texCoordData[prim.indices[2]];
}