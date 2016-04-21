#pragma once

#include "common.h"
#include "Scene.h"
#include "shading.h"

struct Renderer {

public:
	//Renderer();
	//~Renderer();

	virtual void getPixels(Color* pixels, int width, int height, Scene* Scene) = 0;

	/*
#ifdef EMBREE_RENDERER
	RTCDevice device;
	//TODO: Add multiple rendering functions (ray, intersections, shading)
#endif

private:

#ifdef EMBREE_RENDERER
	void defineRay(RTCRay*, const Vec3&, const Vec3&);
	void defineRay(RTCRay8*, const int index, const Vec3&, const Vec3&);
	Color getRayColor(RTCRay*, Scene*, int iteration = 0);
	Color getRayColor(RTCRay8*, const int index, Scene*, int iteration = 0);
#endif
	*/
};

struct EmbreeHandler {

	EmbreeHandler();
	~EmbreeHandler();
	RTCDevice device;

};

struct EmbreeRenderer : Renderer {

	EmbreeRenderer(EmbreeHandler* handler);
	void getPixels(Color* pixels, int width, int height, Scene* Scene);

	EmbreeHandler* handler;

};
