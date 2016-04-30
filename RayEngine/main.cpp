#include "rayengine.h"

float frand() {
	return (float)rand() / RAND_MAX;
}

// Main entry of the program.
// Scenes are constructed using the RayEngine object.
int main(int argc, char **argv) {

	RayEngine rayEngine;

	Scene* myScene = rayEngine.createScene("myScene");
	myScene->camera.position = { 90.9064f, 148.59f, 202.493f };
	myScene->camera.xaxis = { 0.805529f, -0.00260918f, -0.592553f };
	myScene->camera.yaxis = { -0.393318f, 0.745581f, -0.537971f };
	myScene->camera.zaxis = { -0.443199f, -0.666411f, -0.599562f };

	// Add some objects...
	myScene->loadObject("obj/floor.obj");
	Object* teapot = myScene->loadObject("obj/teapot.obj");
	teapot->scale({ 0.5f, 1.5f, 0.5f });
	teapot->rotate({ 0, 1, 0 }, 180.f);

	for (float x = 40.f; x < 400.f; x += 10.f) {
		Object* clone = myScene->addObject(teapot->geometries[0]);
		clone->scale({ 0.75f + frand() * 0.75f, 0.75f + frand() * 0.75f, 0.75f + frand() * 0.75f });
		clone->rotate({ 0, 1, 0 }, frand() * 360.f);
		clone->translate({ x, 0.f, 0.f });
		clone->rotate({ 0, 1, 0 }, frand() * 360.f);
	}
	//myScene->loadObject("Cornell Box", "obj/cornell_box.obj");
	//myScene->loadObject("SLS", "C:/Users/david/Documents/Visual Studio 2013/Projects/SimpleEmbree/Meshes/SLS/sls_amg.obj");

	rayEngine.launch();

}