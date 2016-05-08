#include "rayengine.h"

float frand() {
	return (float)rand() / RAND_MAX;
}

// Main entry of the program.
// Scenes are constructed using the RayEngine object.
int main(int argc, char **argv) {

	RayEngine rayEngine;

	/// Teapot scene /// 

	/*Scene* teapotScene = rayEngine.createScene("myScene", { 0.1f, 0.1f, 0.2f }, "img/CedarCity.ppm");
	teapotScene->camera.position = { 90.9064f, 148.59f, 202.493f };
	teapotScene->camera.xaxis = { 0.805529f, -0.00260918f, -0.592553f };
	teapotScene->camera.yaxis = { -0.393318f, 0.745581f, -0.537971f };
	teapotScene->camera.zaxis = { -0.443199f, -0.666411f, -0.599562f };
	teapotScene->addLight({ 100.f, 200.f, 100.f }, { 1.f, 1.f, 1.f }, 1000.f);

	teapotScene->loadObject("obj/floor.obj");
	Object* teapot = teapotScene->loadObject("obj/teapot.obj");
	teapot->scale({ 0.5f, 1.5f, 0.5f });
	teapot->rotate({ 0, 1, 0 }, 180.f);

	for (float x = 40.f; x < 400.f; x += 20.f) {
		Object* clone = teapotScene->addObject(teapot->geometries[0]);
		clone->scale({ 0.75f + frand() * 0.75f, 0.75f + frand() * 0.75f, 0.75f + frand() * 0.75f });
		clone->rotate({ 0, 1, 0 }, frand() * 360.f);
		clone->translate({ x, 0.f, 0.f });
		clone->rotate({ 0, 1, 0 }, frand() * 360.f);
	}*/

	Scene* cornellBox = rayEngine.createScene("Cornell Box", { 0.1f, 0.1f, 0.2f });
	cornellBox->loadObject("obj/cornell_box.obj");
	cornellBox->addLight({ 280.f, 540.f, 280.f }, { 1.f, 1.f, 1.f }, 1000.f);
	cornellBox->camera.position = { 290.f, 280.f, -290.f };
	cornellBox->camera.xaxis = { -1.f, 0.f, 0.f };
	cornellBox->camera.yaxis = { 0.f, 1.f, 0.f };
	cornellBox->camera.zaxis = { 0.f, 0.f, 1.f };

	/*Scene* sponzaScene = rayEngine.createScene("Sponza", { 0.1f, 0.1f, 0.2f });
	sponzaScene->loadObject("obj/sponza/sponza.obj");
	sponzaScene->addLight({ -6.0f, 6.7f, -0.28f  }, { 1.f, 1.f, 1.f }, 1000.f);
	sponzaScene->camera.position = { -6.0f, 8.7f, -0.28f };*/

	//myScene->loadObject("C:/Users/david/Documents/Visual Studio 2013/Projects/SimpleEmbree/Meshes/SLS/sls_amg.obj"); // Too big for laptop GPU
	//myScene->loadObject("C:/Users/david/Documents/Visual Studio 2013/Projects/SimpleEmbree/Meshes/FutHouse/Futuristic rural house.obj"); // Too big for laptop GPU
	//myScene->loadObject("obj/conference/conference.obj");

	rayEngine.launch();

}