#pragma once

#include "common.h"
#include "camera.h"
#include "object.h"
#include "light.h"

struct Scene {

	Scene(string name);

	string name;

	void initEmbreeScene(RTCDevice device);

	Camera camera;
	vector<Object> objects;
	//map<int, Object&> objectsMap;
	vector<Material*> materials;
	vector<Light*> lights;
	Color ambient, background;
	RTCScene embreeScene;

};
