#pragma once

#include "common.h"
#include "camera.h"

struct Path {
	
	Path(float speed);
	virtual void update(Camera& camera, bool animate) = 0;

	float progress;
	float time;
	bool done;

};

struct LinePath : Path {

	LinePath(Vec3 start, Vec3 end, float time = 10.f);
	void update(Camera& camera, bool animate);

	Vec3 start, end;

};

struct CirclePath : Path {

	CirclePath(Vec3 position, float radius, float angle, float time = 10.f);
	void update(Camera& camera, bool animate);

	Vec3 position;
	float radius, angle;

};

struct LookPath : Path {

	LookPath(float time = 10.f);
	void update(Camera& camera, bool animate);

};