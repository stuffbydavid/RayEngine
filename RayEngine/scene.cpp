#include "Scene.h"

Scene::Scene(string name, Color ambientColor, Color backgroundColor) :
    name(name),
	ambientColor(ambientColor),
	backgroundColor(backgroundColor)
{}

Object* Scene::loadObject(string file) {

	Object* obj = Object::load(file);
	objects.push_back(obj);
	return obj;

}

Object* Scene::addObject(Geometry* geometry) {

	Object* obj = new Object();
	if (geometry)
		obj->geometries.push_back(geometry);
	objects.push_back(obj);
	return obj;

}

Light* Scene::addLight(Vec3 position, Color color, float range) {

	Light* light = new Light(position, color, range);
	lights.push_back(light);
	return light;

}