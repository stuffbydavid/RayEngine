#include "rayengine.h"
#include "triangle_mesh.h"
#include "object.h"

void RayEngine::initEmbree() {

	EmbreeData.device = rtcNewDevice(NULL);
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

}

void Object::initEmbree(RTCDevice device) {

	// Init embree for mesh and all the children
	EmbreeData.scene = rtcDeviceNewScene(device,
										 RTC_SCENE_DYNAMIC | RTC_SCENE_COMPACT,
										 RTC_INTERSECT1 | RTC_INTERSECT4 | RTC_INTERSECT8);
	if (geometry)
		geometry->initEmbree(EmbreeData.scene);

	for (uint i = 0; i < children.size(); i++) {
		children[i]->initEmbree(device);
		children[i]->EmbreeData.instID = rtcNewInstance2(EmbreeData.scene, children[i]->EmbreeData.scene);
	}
	//TODO: Check other instance modes

	rtcCommit(EmbreeData.scene);

}

void TriangleMesh::initEmbree(RTCScene scene) {

	EmbreeData.geomID = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, primitives.size(), posData.size());
	rtcSetBuffer(scene, EmbreeData.geomID, RTC_INDEX_BUFFER, &primitives[0], 0, sizeof(TrianglePrimitive));
	rtcSetBuffer(scene, EmbreeData.geomID, RTC_VERTEX_BUFFER, &posData[0], 0, sizeof(Vec3));

}