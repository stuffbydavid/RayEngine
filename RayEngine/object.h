#pragma once

#include "util.h"
#include "mat4x4.h"
#include "vec3.h"

#include "geometry.h"
#include "material.h"
#include "triangle_mesh.h"

struct Object {

	// Constructor
	Object();
	~Object();

	// Loads object(s) from a file.
	static Object* load(string file);

	// Translates the object by a vector.
	Object* translate(Vec3 vector);

	// Rotates the object by a vector and angle.
	Object* rotate(Vec3 vector, float angle);

	// Scales the object by a vector.
	Object* scale(Vec3 vector);

	// Variables
	vector<Geometry*> geometries;
	Mat4x4 matrix;

	// Embree
	struct Embree {
		RTCScene scene;
		map<uint, Geometry*> geomIDmap;
	} Embree;
	void embreeInit(RTCDevice device);

	// OptiX
	struct Optix{
		optix::Transform transform;
		optix::GeometryGroup geometryGroup;
	} Optix;
	void optixInit(optix::Context context);

};
