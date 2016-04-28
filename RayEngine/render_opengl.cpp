#include "rayengine.h"

void RayEngine::renderOpenGL() {

	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Mat4x4 proj = curCamera->getMatrix(window.ratio);

	for (uint i = 0; i < curScene->objects.size(); i++)
		curScene->objects[i]->renderOpenGL(shdrOGL, proj);

}

void Object::renderOpenGL(Shader* shader, Mat4x4 proj) {

	for (uint i = 0; i < geometries.size(); i++)
		shader->use((TriangleMesh*)geometries[i], proj, this);

}