#include "rayengine.h"

void RayEngine::renderOpenGL() {

	glClearColor(0.3f, 0.9f, 0.3f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Mat4x4 proj = curCamera->getMatrix(window.ratio);

	for (uint i = 0; i < curScene->objects.size(); i++) {
		Object* obj = curScene->objects[i];
		for (uint i = 0; i < obj->geometries.size(); i++)
			OPENGL_SHADER->use(proj * obj->matrix, obj, (TriangleMesh*)obj->geometries[i]);
	}

}