#pragma once

#include "common.h"
#include "camera.h"
#include "object.h"
#include "light.h"

struct Scene {

	Scene(string name);

	string name;

	void initEmbreeScene(RTCDevice device);

	// Loads object(s) from a file and adds it to the scene.
	// If more than one object is available in the file, a root object is
	// returned with all the file objects as children.
	Object* loadObject(string file, Material* defaultMaterial = nullptr);

	Camera camera;
	Object root;
	//map<int, Object*> objectsMap; // Put in EmbreeHandler?
	vector<Material*> materials;
	vector<Light*> lights;
	Color ambient, background;
	RTCScene embreeScene;

};
