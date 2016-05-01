#pragma once

#include "common.h"
#include "geometry.h"
#include "triangle_mesh.h"
#include "material.h"
#include "shader.h"

struct Object {

	// Constructor
	Object();
	~Object();

	// Loads object(s) from a file.
	static Object* load(string file);
	
	// Renders using an OpenGL shader.
	void renderOpenGL(Shader* shader, Mat4x4 proj);

	// Translates the object by a vector.
	void translate(Vec3 vector);

	// Rotates the object by a vector and angle.
	void rotate(Vec3 vector, float angle);

	// Scales the object by a vector.
	void scale(Vec3 vector);

	// Variables
	vector<Geometry*> geometries;
	Mat4x3 matrix;

	// Embree
	struct {
		RTCScene scene;
		map<uint, Geometry*> geomIDmap;
	} EmbreeData;
	void initEmbree(RTCDevice device);

	// OptiX
	struct {
		optix::Transform transform;
		optix::GeometryGroup geometryGroup;
	} OptixData;
	void initOptix(optix::Context context);

};
