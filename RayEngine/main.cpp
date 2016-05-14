#include "rayengine.h"


// Main entry of the program.
// Scenes are constructed using the RayEngine object.
int main(int argc, char **argv) {

	RayEngine rayEngine;

	//// Sponza ////

#if 1
	Scene* sponzaScene = rayEngine.createScene("Sponza", "img/CedarCity.ppm", { 0.1f, 0.1f, 0.2f }, 1.f);
	sponzaScene->loadObject("obj/sponza/sponza.obj");
	sponzaScene->addLight({ -6.0f, 6.7f, -0.28f  }, { 1.f, 1.f, 1.f }, 1000.f);
	sponzaScene->camera.position = { -6.0f, 8.7f, -0.28f };
#endif

	//// Cars ////

#if 0
	Scene* cars = rayEngine.createScene("cars", "img/CedarCity.ppm", { 0.1f, 0.1f, 0.2f });
	cars->loadObject("obj/floor.obj");
	cars->addLight({ 100.f, 200.f, 100.f }, { 1.f, 1.f, 1.f }, 1000.f);
	Object* car = cars->loadObject("C:/Users/david/Documents/Visual Studio 2013/Projects/SimpleEmbree/Meshes/SLS/sls_amg.obj") // Too big for laptop GPU
	->scale({ 50.f, 50.f, 50.f })
	->rotate({ 1, 0, 0 }, -90.f)
	->translate({ 0.f, 24.f, 0.f });

	for (int i = 0; i < 360; i += 30) {
		Object* clone = cars->addObject();
		clone->geometries = car->geometries;
		clone->scale({ 50.f });
		clone->rotate({ 1, 0, 0 }, -90.f);
		clone->translate({ 500.f, 24.f, 0.f });
		clone->rotate({ 0, 1, 0 }, i);
	}

#endif

	//// House ////

#if 0
	Scene* houseScene = rayEngine.createScene("House",, "img/rasen.png" { 0.2f });
	houseScene->loadObject("C:/Users/david/Documents/Visual Studio 2013/Projects/SimpleEmbree/Meshes/FutHouse/Futuristic rural house.obj"); // Too big for laptop GPU
	houseScene->addLight({ 100.f, 1000.f, 100.f }, { 1.f }, 2000.f);*/
#endif

	//// Conference room ///

#if 0
	Scene* conferenceScene = rayEngine.createScene("conference room", "", { 0.5f });
	conferenceScene->loadObject("obj/conference/conference.obj");
	conferenceScene->addLight({ 0, 500, 0 }, { 1.f }, 10000.f);
#endif

	//// Glass ///

	Scene* glassScene = rayEngine.createScene("Glass", "img/rasen.png", { 0.4f });
	glassScene->loadObject("obj/floor.obj");
	glassScene->loadObject("obj/box/box.obj")->rotate({ 0, 1, 0 }, 45)->rotate({ 1, 0, 0 }, 45)->rotate({ 0, 0, 1 }, 45)->scale(10.f)->translate({ -40.f, 15.f, -50.f });
	glassScene->loadObject("obj/Glass/GlassPack.obj")->scale(0.1f);
	glassScene->loadObject("obj/teapot.obj")->translate({ 20.f, 0.f, -50.f });
	glassScene->loadObject("obj/bunny.obj")->scale(150.f)->translate({ 20.f, -5.f, 50.f });
	glassScene->addLight({ 100.f, 200.f, 100.f }, { 1.f, 1.f, 1.f }, 1000.f);

	//// Teapots ///

	Scene* teapotScene = rayEngine.createScene("Teapots", "img/CedarCity.ppm", { 0.1f, 0.1f, 0.2f });
	teapotScene->camera.position = { 90.9064f, 148.59f, 202.493f };
	teapotScene->camera.xaxis = { 0.805529f, -0.00260918f, -0.592553f };
	teapotScene->camera.yaxis = { -0.393318f, 0.745581f, -0.537971f };
	teapotScene->camera.zaxis = { -0.443199f, -0.666411f, -0.599562f };
	teapotScene->addLight({ 100.f, 200.f, 100.f }, { 1.f, 1.f, 1.f }, 1000.f);

	teapotScene->loadObject("obj/floor.obj");
	Object* teapot = teapotScene->loadObject("obj/teapot.obj")
		->scale({ 0.5f, 1.5f, 0.5f })
		->rotate({ 0, 1, 0 }, 180.f);

	for (float x = 40.f; x < 400.f; x += 20.f) {
		Object* clone = teapotScene->addObject(teapot->geometries[0])
			->scale({ 0.75f + frand() * 0.75f, 0.75f + frand() * 0.75f, 0.75f + frand() * 0.75f })
			->rotate({ 0, 1, 0 }, frand() * 360.f)
			->translate({ x, 0.f, 0.f })
			->rotate({ 0, 1, 0 }, frand() * 360.f);
	}

	//// Cornell Box ///

	Scene* cornellBox = rayEngine.createScene("Cornell Box", "", { 0.1f, 0.1f, 0.2f }, 100.f);
	cornellBox->loadObject("obj/cornell_box.obj");
	cornellBox->addLight({ 280.f, 540.f, 280.f }, { 1.f, 1.f, 1.f }, 1000.f);
	cornellBox->camera.position = { 290.f, 280.f, -290.f };
	cornellBox->camera.xaxis = { -1.f, 0.f, 0.f };
	cornellBox->camera.yaxis = { 0.f, 1.f, 0.f };
	cornellBox->camera.zaxis = { 0.f, 0.f, 1.f };

	rayEngine.launch();

}