#pragma once

#include "common.h"
#include "camera.h"
#include "object.h"
#include "light.h"

struct Scene {

	Scene(string name);

	// Loads object(s) from a file and adds it to the scene.
	Object* loadObject(string name, string file, Material* defaultMaterial = nullptr);

	string name;
	Camera camera;
	vector<Object*> objects;
	vector<Light*> lights;
	Color ambient, background;

	//map<int, Object*> objectsMap; // TODO: Put in EmbreeHandler?

	// Embree
	struct {
		RTCScene scene;
	} EmbreeData;
	void initEmbree(RTCDevice device);

	// OptiX
	struct {
		optix::Group group;
	} OptixData;
	void initOptix(optix::Context context);

};
