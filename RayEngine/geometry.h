// Stores a mesh or custom geometry

#pragma once
#include "common.h"

struct Geometry {

	enum GeometryType
	{
		TriangleMesh,
		QuadMesh,
		Custom
	};

	virtual GeometryType getGeometryType() = 0;

};
