#include "rayengine.h"

RayEngine::Timer::Timer() :
	avgTime(0.f),
	lastTime(0.f),
	startMeasure(0.f),
	avgTotal(0.f),
	lastAvgMeasure(0.f),
	avgAdditions(0)
{
}


void RayEngine::Timer::start() {
	startMeasure = glfwGetTime();
}

void RayEngine::Timer::stop() {

	lastTime = glfwGetTime() - startMeasure;
	avgTotal += lastTime;
	avgAdditions++;
	if (glfwGetTime() - lastAvgMeasure > 1.f) {
		avgTime = avgTotal / avgAdditions;
		avgTotal = 0.f;
		avgAdditions = 0;
		lastAvgMeasure = glfwGetTime();
	}

}