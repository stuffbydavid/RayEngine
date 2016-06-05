#include "rayengine.h"

void RayEngine::logInit() {

	logFileName = LOG_FILE(date());
	logFileStream.open(logFileName, ios::out | ios::app);
	LOG("Program start");
	LOG("Settings:");
	for (Setting* s : settings)
		LOG("\t" + s->getLogText());
}

void RayEngine::logClose() {

	logFileStream.close();

}

void RayEngine::benchmarkStart() {

	benchmarkMode = true;
	LOG(date() + " Started benchmark");

	if (curScene->cameraPath)
		curScene->cameraPath->frame = 0.f;

}

void RayEngine::benchmarkStop() {

	benchmarkMode = false;
	LOG(date() + " Stopped benchmark");

}

void RayEngine::benchmarkUpdate() {

	if (renderMode == RM_OPENGL)
		return;

	string frameColumn = "";

	if (curScene->cameraPath) {
		
		if (curScene->cameraPath->frame >= BENCHMARK_TARGET_FPS * curScene->cameraPath->time) {
			benchmarkStop();
			return;
		}

		frameColumn = to_string_prec(curScene->cameraPath->frame, 0) + "\t";

	}

	if (renderMode == RM_EMBREE)
		LOG(frameColumn + to_string_prec(Embree.renderTimer.lastTime, 4));
	else if (renderMode == RM_OPTIX)
		LOG(frameColumn + to_string_prec(Optix.renderTimer.lastTime, 4));
	else
		LOG(
			frameColumn +
			to_string_prec(Hybrid.renderTimer.lastTime, 4) + "\t" +
			to_string_prec(Embree.renderTimer.lastTime, 4) + "\t" +
			to_string_prec(Optix.renderTimer.lastTime, 4)
		);

}