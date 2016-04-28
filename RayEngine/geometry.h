#pragma once

#include "common.h"

// Stores a mesh or custom geometry
struct Geometry {

	// Embree
	virtual uint initEmbree(RTCScene scene) = 0;

	// OptiX
	struct {
		optix::Geometry geometry;
		optix::GeometryInstance geometryInstance;
	} OptixData;
	virtual void initOptix(optix::Context context) = 0;

};
