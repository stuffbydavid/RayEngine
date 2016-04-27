#include "Scene.h"

Scene::Scene(string name) : name(name) {}

Object* Scene::loadObject(string file, Material* defaultMaterial) {

	Object* obj = Object::load(file, defaultMaterial);
	root.children.push_back(obj);
	return obj;

}