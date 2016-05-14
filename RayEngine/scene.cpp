#include "Scene.h"

Scene::Scene(string name, string skyFile, Color ambient, float aoRadius, Color skyColor) :
    name(name),
	ambient(ambient),
	aoRadius(aoRadius)
{
	if (skyFile != "")
		sky = new Image(GL_LINEAR, skyFile);
	else
		sky = new Image(skyColor);
}

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

void Scene::addLight(Vec3 position, Color color, float range) {

	lights.push_back({ position, color, range });

}