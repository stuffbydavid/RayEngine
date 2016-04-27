#include "Scene.h"

Scene::Scene(string name) : name(name) {}

Object* Scene::loadObject(string name, string file, Material* defaultMaterial) {

	Object* obj = Object::load(name, file, defaultMaterial);
	objects.push_back(obj);
	return obj;

}