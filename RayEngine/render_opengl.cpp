#include "rayengine.h"

void RayEngine::renderOpenGL() {

	Mat4x4 proj = scenes[0]->camera.getMatrix((float)window.width / window.height);

	for (uint i = 0; i < scenes[0]->objects.size(); i++)
		scenes[0]->objects[i]->renderOpenGL(shdrOGL, proj);

}

void Object::renderOpenGL(Shader* shader, Mat4x4 proj) {

	for (uint i = 0; i < geometries.size(); i++)
		shader->use((TriangleMesh*)geometries[i], proj, this);

}