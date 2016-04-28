#include "rayengine.h"

// Main entry of the program.
// Scenes are constructed from outside the RayEngine object.
int main(int argc, char **argv) {

	RayEngine rayEngine;

	Scene* myScene = rayEngine.createScene("myScene");

	// Add some objects...
	myScene->loadObject("Floor", "obj/floor.obj");
	myScene->loadObject("Teapot", "obj/teapot.obj");
	//myScene->loadObject("Cornell Box", "obj/cornell_box.obj");
	//myScene->loadObject("SLS", "C:/Users/david/Documents/Visual Studio 2013/Projects/SimpleEmbree/Meshes/SLS/sls_amg.obj");

	rayEngine.launch();

}