#include "rayengine.h"
#include "triangle_mesh.h"
#include "object.h"

void* userData;

void RayEngine::embreeInit() {

	cout << "Starting Embree..." << endl;

	// Init library
	Embree.device = rtcNewDevice(NULL);
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	// Generate texture
	glGenTextures(1, &Embree.texture);
	glBindTexture(GL_TEXTURE_2D, Embree.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Init scenes
	userData = this;
	for (uint i = 0; i < scenes.size(); i++)
		scenes[i]->embreeInit(Embree.device);

}

void Scene::embreeInit(RTCDevice device) {

	Embree.scene = rtcDeviceNewScene(device, EMBREE_SFLAGS_SCENE, EMBREE_AFLAGS_SCENE);

	for (uint i = 0; i < objects.size(); i++) {

		objects[i]->embreeInit(device);
		uint instID = rtcNewInstance2(Embree.scene, objects[i]->Embree.scene);
		rtcSetTransform2(Embree.scene, instID, RTC_MATRIX_COLUMN_MAJOR_ALIGNED16, objects[i]->matrix.e);
		Embree.instIDmap[instID] = objects[i];

	}

	rtcCommit(Embree.scene);

}

void Object::embreeInit(RTCDevice device) {

	Embree.scene = rtcDeviceNewScene(device, EMBREE_SFLAGS_OBJECT, EMBREE_AFLAGS_OBJECT);

	// Init embree for meshes
	for (uint i = 0; i < geometries.size(); i++) { // If it crashes here, then it can't find the .mtl or the last line is not empty

		uint geomID = geometries[i]->embreeInit(Embree.scene);
		Embree.geomIDmap[geomID] = geometries[i];

		// Set filter functions
		rtcSetOcclusionFilterFunction(Embree.scene, geomID, (RTCFilterFunc)&RayEngine::embreeOcclusionFilter);
		rtcSetOcclusionFilterFunction8(Embree.scene, geomID, (RTCFilterFunc8)&RayEngine::embreeOcclusionFilter8);
		rtcSetUserData(Embree.scene, geomID, userData);

	}

	rtcCommit(Embree.scene);

}

uint TriangleMesh::embreeInit(RTCScene scene) {

	uint geomID = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, indexData.size(), posData.size());
	rtcSetBuffer(scene, geomID, RTC_VERTEX_BUFFER, &posData[0], 0, sizeof(Vec3));
	rtcSetBuffer(scene, geomID, RTC_INDEX_BUFFER, &indexData[0], 0, sizeof(TrianglePrimitive));
	return geomID;

}

void RayEngine::embreeUpdatePartition() {

	// Set dimensions
	if (renderMode == RM_HYBRID) {
		Embree.offset = 0;
		if (Embree.renderTiles)
			Embree.width = ceil((float)(window.width * Hybrid.partition) / (float)Embree.tileWidth) * Embree.tileWidth;
		else
			Embree.width = ceil(window.width * Hybrid.partition);
	}
	else {
		Embree.offset = 0;
		Embree.width = window.width;
	}

}

void RayEngine::embreeResize() {

	// Resize buffer
	Embree.buffer.resize(window.width * window.height);

	// Resize texture
	glBindTexture(GL_TEXTURE_2D, Embree.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.width, window.height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}