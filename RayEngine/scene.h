#pragma once

#include "common.h"
#include "camera.h"
#include "object.h"
#include "light.h"

struct Scene {

	Scene(string name);

	// Loads object(s) from a file and adds it to the scene.
	Object* loadObject(string file, Material* defaultMaterial = nullptr);

	// Adds a new customizable object
	Object* addObject(Geometry* geometry = nullptr);

	string name;
	Camera camera;
	vector<Object*> objects;
	vector<Light*> lights;
	Color ambient, background;

	// Embree
	struct {
		RTCScene scene;
		map<uint, Object*> instIDmap;
	} EmbreeData;
	void initEmbree(RTCDevice device);

	// OptiX
	struct {
		optix::Group group;
	} OptixData;
	void initOptix(optix::Context context);

};
