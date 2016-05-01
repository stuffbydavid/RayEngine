#include "Scene.h"

Scene::Scene(string name) : name(name) {}

Object* Scene::addObject(Geometry* geometry) {

	Object* obj = new Object();
	if (geometry)
		obj->geometries.push_back(geometry);
	objects.push_back(obj);
	return obj;

}

Object* Scene::loadObject(string file) {

	Object* obj = Object::load(file);
	objects.push_back(obj);
	return obj;

}