#pragma once

#include "common.h"
#include "geometry.h"
#include "triangle_mesh.h"
#include "material.h"
#include "tiny_obj_loader.h"


struct Object {

	Object(string name = "");
	~Object();

	// Loads object(s) from a file. If more than one object is available in the file,
	// a root object is returned with all the file objects as children.
	static Object* load(string file, Material* defaultMaterial = nullptr);

	string name;
	vector<Object*> children;

	struct {
		RTCScene scene;
	} EmbreeData;




	void initEmbreeScene(RTCDevice device);
	int id;
	Geometry* geometry;
	RTCScene embreeScene;
	Mat4x3 M;

};