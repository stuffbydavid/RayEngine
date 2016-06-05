#pragma once

#include "material.h"

#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>

// Stores a mesh or custom geometry
struct Geometry {

	Material* material;

	// Embree
	virtual uint embreeInit(RTCScene scene) = 0;

	// OptiX
	struct {
		optix::Geometry geometry;
		optix::GeometryInstance geometryInstance;
	} Optix;
	virtual void optixInit(optix::Context context) = 0;

};
