#include "rayengine.h"

int main(int argc, char **argv) {

	RayEngine rayEngine;

	Scene* myScene = rayEngine.createScene("myScene");

	// Add some objects...
	myScene->loadObject("obj/cornell_box.obj");


	rayEngine.launch();

}


/*#include "object_loader.h"

Window* pWindow;
Scene* pScene;
Renderer* pRenderer;

void input();

void update() {
	
	input();
	pWindow->setTitle("Render Base, FPS: " + to_string(pWindow->fps));
	pRenderer->getPixels(pWindow->buffer, pWindow->width, pWindow->height, pScene);

}

int main(int argc, char **argv) {

	pWindow = new Window(600, 400);

	Magick::InitializeMagick(NULL);
	EmbreeHandler* pEmbreeHandler;
	EmbreeRenderer* pEmbreeRenderer;

	pEmbreeHandler = new EmbreeHandler();
	pEmbreeRenderer = new EmbreeRenderer(pEmbreeHandler);
	pRenderer = pEmbreeRenderer; 

	pScene = new Scene();
	pScene->camera.position = Vec3(275.f, 275.f, -500.f);
	pScene->camera.xaxis = Vec3(1.f, 0.f, 0.f);
	pScene->camera.yaxis = Vec3(0.f, 1.f, 0.f);
	pScene->camera.zaxis = Vec3(0.f, 0.f, 1.f);
	pScene->camera.fov = 60;
	pScene->lights = { new Light({ 275.f, 448.8f, 275.f }, Color(1, 1, 1, 1)) };
	pScene->ambient = Color(0.1f, 0.15f, 0.15f, 1);
	pScene->background = Color(0.f, 0.f, 0.0f, 1);
	
	ObjectLoader::loadObjects(&pScene->objects, &pScene->materials, "obj\\floor.obj");
	ObjectLoader::loadObjects(&pScene->objects, &pScene->materials, "obj\\cornell_box.obj");
	//ObjectLoader::loadObjects(&pScene->objects, &pScene->materials, "obj\\rabbit\\Rabbit.obj");
	//ObjectLoader::loadObjects(&pScene->objects, &pScene->materials, "obj\\sphere.obj");
	//ObjectLoader::loadObjects(&pScene->objects, &pScene->materials, "C:\\Users\\david\\Documents\\Visual Studio 2013\\Projects\\SimpleEmbree\\Meshes\\SLS\\sls_amg.obj"); // BUGGED TEXTURES
	//ObjectLoader::loadObjects(&pScene->objects, &pScene->materials, "obj\\sponza\\sponza.obj");
	//ObjectLoader::loadObjects(&pScene->objects, &pScene->materials, "obj\\conference\\conference.obj");
	// TODO CARS

	pScene->initEmbreeScene(pEmbreeHandler->device); 

	pWindow->Run(update);

	delete pEmbreeHandler;
	delete pEmbreeRenderer;
	delete pScene;
	delete pWindow;

	return 0; 

}

void input() {

	static bool moveCamera = false;
	Camera* selectedCamera = &pScene->camera;

	// Start/Stop moving

	if (pWindow->mousePressed[GLFW_MOUSE_BUTTON_LEFT]) {
		moveCamera = true;
		glfwSetInputMode(pWindow->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (pWindow->mouseReleased[GLFW_MOUSE_BUTTON_LEFT]) {
		moveCamera = false;
		glfwSetInputMode(pWindow->handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// Move the camera

	if (moveCamera) {

		float lookFactor = 0.2f;
		float moveFactor = 2.f;
		float rotateFactor = 2.f;

		// Rotate X and Z axis around Y axis for yaw

		selectedCamera->xaxis = Mat4x3::rotate(selectedCamera->xaxis, selectedCamera->yaxis, pWindow->mouseMove.x() * lookFactor);
		selectedCamera->zaxis = Mat4x3::rotate(selectedCamera->zaxis, selectedCamera->yaxis, pWindow->mouseMove.x() * lookFactor);

		// Rotate Y and Z axis around X axis for pitch

		selectedCamera->yaxis = Mat4x3::rotate(selectedCamera->yaxis, selectedCamera->xaxis, pWindow->mouseMove.y() * lookFactor);
		selectedCamera->zaxis = Mat4x3::rotate(selectedCamera->zaxis, selectedCamera->xaxis, pWindow->mouseMove.y() * lookFactor);

		// Go faster

		if (pWindow->keyDown[GLFW_KEY_SPACE])
			moveFactor *= 10;

		// Go slower

		if (pWindow->keyDown[GLFW_KEY_LEFT_SHIFT])
			moveFactor /= 10;

		// Move along the axises

		if (pWindow->keyDown[GLFW_KEY_W])
			selectedCamera->position += selectedCamera->zaxis * moveFactor;

		if (pWindow->keyDown[GLFW_KEY_S])
			selectedCamera->position -= selectedCamera->zaxis * moveFactor;

		if (pWindow->keyDown[GLFW_KEY_D])
			selectedCamera->position += selectedCamera->xaxis * moveFactor;

		if (pWindow->keyDown[GLFW_KEY_A])
			selectedCamera->position -= selectedCamera->xaxis * moveFactor;

		// Roll (rotate X and Y axis around Z axis)

		if (pWindow->keyDown[GLFW_KEY_Q]) {
			selectedCamera->xaxis = Mat4x3::rotate(selectedCamera->xaxis, selectedCamera->zaxis, -rotateFactor);
			selectedCamera->yaxis = Mat4x3::rotate(selectedCamera->yaxis, selectedCamera->zaxis, -rotateFactor);
		}

		if (pWindow->keyDown[GLFW_KEY_E]) {
			selectedCamera->xaxis = Mat4x3::rotate(selectedCamera->xaxis, selectedCamera->zaxis, rotateFactor);
			selectedCamera->yaxis = Mat4x3::rotate(selectedCamera->yaxis, selectedCamera->zaxis, rotateFactor);
		}

		// FOV

		if (pWindow->keyDown[GLFW_KEY_T])
			selectedCamera->fov++;

		if (pWindow->keyDown[GLFW_KEY_G])
			selectedCamera->fov--;
	}

}*/