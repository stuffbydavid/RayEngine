#pragma once

#include "common.h"

// Stores a mesh or custom geometry
struct Geometry {

	// Embree
	struct {
		uint geomID;
	} EmbreeData;
	virtual void initEmbree(RTCScene scene) = 0;

	// OptiX
	struct {
		optix::Geometry geometry;
	};
	virtual void initOptix(optix::Context context) = 0;

};
