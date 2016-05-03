#include "rayengine.h"
#include "triangle_mesh.h"
#include "object.h"

void RayEngine::initEmbree() {

	cout << "Starting Embree..." << endl;

	// Init library
	EmbreeData.device = rtcNewDevice(NULL);
	EmbreeData.buffer = nullptr;
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	// Generate texture
	glGenTextures(1, &EmbreeData.texture);
	glBindTexture(GL_TEXTURE_2D, EmbreeData.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Init scenes
	for (uint i = 0; i < scenes.size(); i++)
		scenes[i]->initEmbree(EmbreeData.device);

}

void Scene::initEmbree(RTCDevice device) {

	EmbreeData.scene = rtcDeviceNewScene(device, EMBREE_SFLAGS_SCENE, EMBREE_AFLAGS_SCENE);

	//TODO: Check other instance modes?
	for (uint i = 0; i < objects.size(); i++) {
		objects[i]->initEmbree(device);
		uint instID = rtcNewInstance2(EmbreeData.scene, objects[i]->EmbreeData.scene);
		rtcSetTransform2(EmbreeData.scene, instID, RTC_MATRIX_COLUMN_MAJOR_ALIGNED16, objects[i]->matrix.e);
		EmbreeData.instIDmap[instID] = objects[i];
	}

	rtcCommit(EmbreeData.scene);

}

void Object::initEmbree(RTCDevice device) {

	EmbreeData.scene = rtcDeviceNewScene(device, EMBREE_SFLAGS_OBJECT, EMBREE_AFLAGS_OBJECT);

	// Init embree for meshes
	for (uint i = 0; i < geometries.size(); i++) {
		uint geomID = geometries[i]->initEmbree(EmbreeData.scene);
		EmbreeData.geomIDmap[geomID] = geometries[i];
	}

	rtcCommit(EmbreeData.scene);

}

uint TriangleMesh::initEmbree(RTCScene scene) {

	uint geomID = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, indexData.size(), posData.size());
	rtcSetBuffer(scene, geomID, RTC_VERTEX_BUFFER, &posData[0], 0, sizeof(Vec3));
	rtcSetBuffer(scene, geomID, RTC_INDEX_BUFFER, &indexData[0], 0, sizeof(TrianglePrimitive));
	return geomID;

}

void RayEngine::resizeEmbree() {

	// Set dimensions
	if (rayTracingTarget == RTT_HYBRID) {
		EmbreeData.offset = 0;
		EmbreeData.width = ceil(window.width * hybridPartition);
	} else {
		EmbreeData.offset = 0;
		EmbreeData.width = window.width;
	}

	// Resize buffer
	if (EmbreeData.buffer)
		delete EmbreeData.buffer;
	EmbreeData.buffer = new Color[EmbreeData.width * window.height];

	// Resize texture
	glBindTexture(GL_TEXTURE_2D, EmbreeData.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, EmbreeData.width, window.height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}