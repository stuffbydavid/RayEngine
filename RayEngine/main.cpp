#include "rayengine.h"


// Main entry of the program.
// Scenes are constructed using the RayEngine object.
int main(int argc, char **argv) {

	RayEngine rayEngine;

	//// Sponza ////

#if 1
	Scene* sponzaScene = rayEngine.createScene("Sponza", "img/night.jpg", { 0.1f, 0.1f, 0.25f }, 35.f);
	sponzaScene->loadObject("obj/sponza/sponza.obj")->scale(20.f);
	sponzaScene->addLight({ 104.74f, 161.505f, -0.820323f }, { 1.f, 1.f, 1.f }, 1000.f);
	sponzaScene->camera.position = { -186.268f, 34.4296f, 20.8563f };
	sponzaScene->camera.xaxis = { 0.328917f, 0.132986f, 0.93495f };
	sponzaScene->camera.yaxis = { -0.253518f, 0.966128f, -0.0482334f };
	sponzaScene->camera.zaxis = { 0.909697f, 0.221163f, -0.35149f };
#endif

	//// Sibenik ////

#if 1
	Scene* sibenikScene = rayEngine.createScene("Sibenik", "img/sky.jpg", { 0.3f, 0.3f, 0.3f }, 45.f);
	sibenikScene->loadObject("obj/sibenik/sibenik.obj")->scale(20.f);
	sibenikScene->addLight({ -6.0f, 1.7f, -0.28f }, { 1.f, 1.f, 1.f }, 1000.f);
	sibenikScene->camera.position = { -363.802f, -275.919f, -2.64324f };
	sibenikScene->camera.xaxis = { 0.0331838f, -0.000354495f, 0.999451f };
	sibenikScene->camera.yaxis = { -0.260939f, 0.965317f, 0.00900495f };
	sibenikScene->camera.zaxis = { 0.964788f, 0.261095f, -0.0319401f };
	sibenikScene->setCameraPath(new LookPath(10.f));
#endif

	//// Cars ////

#if 0
	Scene* cars = rayEngine.createScene("cars", "img/CedarCity.ppm", { 0.1f, 0.1f, 0.2f });
	cars->loadObject("obj/floor.obj");
	cars->addLight({ 100.f, 200.f, 100.f }, { 1.f, 1.f, 1.f }, 1000.f);
	Object* car = cars->loadObject("C:/Users/Cuda/Documents/Models/SLS/sls_amg.obj") // Too big for laptop GPU
	->scale({ 50.f, 50.f, 50.f })
	->rotate({ 1, 0, 0 }, -90.f)
	->translate({ 0.f, 24.f, 0.f });

	/*for (int i = 0; i < 360; i += 30) {
		Object* clone = cars->addObject();
		clone->geometries = car->geometries;
		clone->scale({ 50.f });
		clone->rotate({ 1, 0, 0 }, -90.f);
		clone->translate({ 500.f, 24.f, 0.f });
		clone->rotate({ 0, 1, 0 }, i);
	}*/

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

#if 1
	Scene* glassScene = rayEngine.createScene("Glass", "img/sky.jpg", { 0.4f });
	glassScene->loadObject("obj/floor.obj");
	//glassScene->loadObject("obj/box/box.obj")->rotate({ 0, 1, 0 }, 45)->rotate({ 1, 0, 0 }, 45)->rotate({ 0, 0, 1 }, 45)->scale(10.f)->translate({ -40.f, 15.f, -50.f });
	glassScene->loadObject("obj/Glass/GlassPack.obj")->scale(0.1f);
	glassScene->loadObject("obj/teapot.obj")->scale(0.3f)->translate({ 20.f, 0.f, -30.f });
	glassScene->loadObject("obj/bunny.obj")->scale(200.f)->translate({ -30.f, -8.f, -30.f });
	//glassScene->loadObject("obj/torus.obj")->scale(40.f)->translate({ 0.f, 0.f, 30.f });
	glassScene->addLight({ 100.f, 1000.f, 100.f }, { 1.f, 1.f, 1.f }, 10000.f);
	glassScene->camera.position = { -16.432f, 18.3147f, 43.6698f };
	glassScene->camera.xaxis = { 0.99995f, 0.0023377f, 0.00968323f };
	glassScene->camera.yaxis = { -0.00196563f, 0.999266f, -0.0382575f };
	glassScene->camera.zaxis = { 0.00976554f, -0.0382365f, -0.999222f };
	glassScene->setCameraPath(new CirclePath({ 0.f, 10.f, 0.f }, 80.f, M_PIf / 12.f, 10.f));
#endif

	//// Teapots ///

#if 0
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
			->scale({ 0.25f + frand() * 0.5f, 0.25f + frand() * 0.5f, 0.25f + frand() * 0.5f })
			->rotate({ 0, 1, 0 }, frand() * 360.f)
			->translate({ x, 0.f, 0.f })
			->rotate({ 0, 1, 0 }, frand() * 360.f);
	}
#endif

	//// Cornell Box ///

#if 0
	Scene* cornellBox = rayEngine.createScene("Cornell Box", "", { 0.1f, 0.1f, 0.2f }, 100.f);
	cornellBox->loadObject("obj/cornell_box.obj");
	cornellBox->addLight({ 280.f, 540.f, 280.f }, { 1.f, 1.f, 1.f }, 1000.f);
	cornellBox->camera.position = { 290.f, 280.f, -290.f };
	cornellBox->camera.xaxis = { -1.f, 0.f, 0.f };
	cornellBox->camera.yaxis = { 0.f, 1.f, 0.f };
	cornellBox->camera.zaxis = { 0.f, 0.f, 1.f };
#endif

	rayEngine.launch();

}