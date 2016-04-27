#include "rayengine.h"

void RayEngine::renderOpenGL() {

	Mat4x4 proj = scenes[0]->camera.getMatrix((float)window.width / window.height);
	scenes[0]->root.render(shdrOGL, proj);

}