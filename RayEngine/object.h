#pragma once

#include "common.h"
#include "geometry.h"
#include "triangle_mesh.h"
#include "material.h"
#include "shader.h"

struct Object {

	// Constructor
	Object(string name = "");
	~Object();

	// Loads object(s) from a file.
	static Object* load(string name, string file, Material* defaultMaterial = nullptr);
	
	// Renders using an OpenGL shader.
	void renderOpenGL(Shader* shader, Mat4x4 proj);

	// Variables
	string name;
	vector<Geometry*> geometries;

	// Embree
	struct {
		RTCScene scene;
		uint instID;
	} EmbreeData;
	void initEmbree(RTCDevice device);

	// OptiX
	struct {
		optix::Transform transform;
		optix::GeometryGroup geometryGroup;
	} OptixData;
	void initOptix(optix::Context context);

};

typedef map<uint, Object*> ObjectMap;