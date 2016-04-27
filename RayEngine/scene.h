#pragma once

#include "common.h"
#include "camera.h"
#include "object.h"
#include "light.h"

struct Scene {

	Scene(string name);

	// Loads object(s) from a file and adds it to the scene.
	// If more than one object is available in the file, a parent object is
	// returned with all the file objects as children.
	Object* loadObject(string file, Material* defaultMaterial = nullptr);

	string name;
	Camera camera;
	Object root;

	//map<int, Object*> objectsMap; // TODO: Put in EmbreeHandler?

	vector<Light*> lights;
	Color ambient, background;

};
