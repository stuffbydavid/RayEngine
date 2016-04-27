#include "rayengine.h"

// Main entry of the program.
// Scenes are constructed from outside the RayEngine object.
int main(int argc, char **argv) {

	RayEngine rayEngine;

	Scene* myScene = rayEngine.createScene("myScene");

	// Add some objects...
	myScene->loadObject("Teapot", "obj/teapot.obj");
	//myScene->loadObject("obj/cornell_box.obj");
	//myScene->loadObject("C:/Users/david/Documents/Visual Studio 2013/Projects/SimpleEmbree/Meshes/SLS/sls_amg.obj");

	rayEngine.launch();

}