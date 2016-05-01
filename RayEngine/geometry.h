#pragma once

#include "common.h"
#include "material.h"

// Stores a mesh or custom geometry
struct Geometry {

	Material* material;

	// Embree
	virtual uint initEmbree(RTCScene scene) = 0;

	// OptiX
	struct {
		optix::Geometry geometry;
		optix::GeometryInstance geometryInstance;
	} OptixData;
	virtual void initOptix(optix::Context context) = 0;

};
