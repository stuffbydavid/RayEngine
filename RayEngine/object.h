#pragma once

#include "common.h"
#include "geometry.h"
#include "triangle_mesh.h"
#include "material.h"
#include "shader.h"

struct Object {

	Object(string name = "");
	~Object();

	// Loads object(s) from a file. If more than one object is available in the file,
	// a root object is returned with all the file objects as children.
	static Object* load(string file, Material* defaultMaterial = nullptr);
	
	void render(Shader* shader, Mat4x4 proj);

	string name;
	vector<Object*> children;
	Geometry* geometry;

	// Embree
	struct {
		RTCScene scene;
		uint instID;
	} EmbreeData;
	void initEmbree(RTCDevice device);

};

typedef map<uint, Object*> ObjectMap;