#pragma once

#include "common.h"
#include "camera.h"
#include "object.h"
#include "light.h"

struct Scene {

	Scene(string name, Color ambient, string skyFile, Color skyColor);

	// Loads object(s) from a file and adds it to the scene.
	Object* loadObject(string file);

	// Adds a new customizable object.
	Object* addObject(Geometry* geometry = nullptr);

	// Adds a new light.
	void addLight(Vec3 position, Color color, float range);

	//// Variables ////

	string name;
	Color ambient;
	Image* sky;
	Camera camera;
	vector<Object*> objects;
	vector<Light> lights;

	//// Embree ////

	struct Embree {
		RTCScene scene;
		map<uint, Object*> instIDmap;
	} Embree;

	void embreeInit(RTCDevice device);

	//// OptiX ////

	struct Optix {
		optix::Group group;
		optix::TextureSampler sky;
	} Optix;

	void optixInit(optix::Context context);

};
