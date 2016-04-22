#include "Scene.h"

Scene::Scene(string name) : name(name) {}

void Scene::initEmbreeScene(RTCDevice device) {
	/*
	RTCSceneFlags sFlags = RTC_SCENE_DYNAMIC | RTC_SCENE_COMPACT;
	RTCAlgorithmFlags aFlags = RTC_INTERSECT1 | RTC_INTERSECT4 | RTC_INTERSECT8;
	embreeScene = rtcDeviceNewScene(device, sFlags, aFlags);
	for (int i = 0; i < objects.size(); i++) {
		objects[i].initEmbreeScene(device);
		objects[i].id = rtcNewInstance(embreeScene, objects[i].embreeScene);
		//objectsMap[objects[i].id] = objects[i];
	}
	rtcCommit(embreeScene);*/

}

Object* Scene::loadObject(string file, Material* defaultMaterial) {

	Object* obj = Object::load(file, defaultMaterial);
	root.children.push_back(obj);
	return obj;

}