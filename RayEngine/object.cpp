#include "object.h"

Object::Object() {

}
Object::~Object() {
	//delete geometry;
}

void Object::initEmbreeScene(RTCDevice device) {

	//TODO: Check other instance modes
	RTCSceneFlags sFlags = RTC_SCENE_DYNAMIC | RTC_SCENE_COMPACT;
	RTCAlgorithmFlags aFlags = RTC_INTERSECT1 | RTC_INTERSECT4 | RTC_INTERSECT8;

	embreeScene = rtcDeviceNewScene(device, sFlags, aFlags);

	switch (geometry->getGeometryType()) {
		case Geometry::TriangleMesh:

			TriangleMesh* pMesh = (TriangleMesh*)geometry;
			pMesh->id = rtcNewTriangleMesh(embreeScene, RTC_GEOMETRY_STATIC, pMesh->primitives.size(), pMesh->posData.size());

			struct Vertex   { float x, y, z, a; };
			struct Triangle { int v0, v1, v2; };

			Vertex* vertices = (Vertex*)rtcMapBuffer(embreeScene, pMesh->id, RTC_VERTEX_BUFFER);
			// fill vertices here
			for (int i = 0; i < pMesh->posData.size(); i++) {
				vertices[i].x = pMesh->posData[i].x();
				vertices[i].y = pMesh->posData[i].y();
				vertices[i].z = pMesh->posData[i].z();
				vertices[i].a = 0;
			}
			rtcUnmapBuffer(embreeScene, pMesh->id, RTC_VERTEX_BUFFER);

			Triangle* triangles = (Triangle*)rtcMapBuffer(embreeScene, pMesh->id, RTC_INDEX_BUFFER);
			// fill triangle indices here
			for (int i = 0; i < pMesh->primitives.size(); i++) {
				triangles[i].v0 = pMesh->primitives[i].indices[0];
				triangles[i].v1 = pMesh->primitives[i].indices[1];
				triangles[i].v2 = pMesh->primitives[i].indices[2];
			}
			rtcUnmapBuffer(embreeScene, pMesh->id, RTC_INDEX_BUFFER);

			break;
	}

	rtcCommit(embreeScene);

}
