#pragma once

#include "common.h"
#include "geometry.h"
#include "triangle_mesh.h"
#include "material.h"

struct Object {

	Object();
	~Object();

	void initEmbreeScene(RTCDevice device);

	int id;
	string name;
	Geometry* geometry;
	RTCScene embreeScene;
	Mat4x3 M;

	vector<Object> objects;

};
