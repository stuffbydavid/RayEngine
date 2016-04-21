#include "window.h"

Window* w;

void windowMouseButtonCallback(GLFWwindow* handle, int button, int action, int mods) {

	// A mouse button was pressed!

	if (button < 0)
		return;

	if (action == GLFW_PRESS) {
		if (!w->mouseDown[button])
			w->mousePressed[button] = true;
		w->mouseDown[button] = true;
	} else if (action == GLFW_RELEASE) {
		if (w->mouseDown[button])
			w->mouseReleased[button] = true;
		w->mouseDown[button] = false;
	}

}

void windowCursorPosCallback(GLFWwindow* handle, double x, double y) {

	// Mouse was moved!

	w->mousePrevious = w->mouse;
	w->mouse = { (float)x, (float)y };
	w->mouseMove =  w->mouse - w->mousePrevious;

}

void windowKeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods) {

	// A keyboard key was pressed!

	if (key < 0)
		return;

	if (action == GLFW_PRESS) {
		if (!w->keyDown[key])
			w->keyPressed[key] = true;
		w->keyDown[key] = true;
	} else if (action == GLFW_RELEASE) {
		if (w->keyDown[key])
			w->keyReleased[key] = true;
		w->keyDown[key] = false;
	}

}

void windowSizeCallback(GLFWwindow* handle, int width, int height) {

	/*if (w->buffer)
		delete w->buffer;
	w->buffer = new Color[width * height];*/

}

void Window::init(int width, int height) {

	glfwInit();

	// Initialize input

	w = this;
	for (uint k = 0; k < GLFW_KEY_LAST; k++) {
		keyDown[k] = false;
		keyPressed[k] = false;
		keyReleased[k] = false;
	}
	for (uint m = 0; m < GLFW_MOUSE_BUTTON_LAST; m++) {
		mouseDown[m] = false;
		mousePressed[m] = false;
		mouseReleased[m] = false;
	}

	// Initialize window

	handle = glfwCreateWindow(width, height, "", NULL, NULL);
	glfwSetMouseButtonCallback(handle, windowMouseButtonCallback);
	glfwSetCursorPosCallback(handle, windowCursorPosCallback);
	glfwSetKeyCallback(handle, windowKeyCallback);
	glfwSetWindowSizeCallback(handle, windowSizeCallback);
	windowSizeCallback(handle, width, height);
	glfwMakeContextCurrent(handle);

	// Init GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		cout << "ERROR";

	// Generate buffers
	glGenBuffers(1, &vbo);
	glGenTextures(1, &texture);

}

void Window::open(function<void(void)> updateFunc) {

	int lastTime = -1, frame = 0;
	fps = 0;

	while (!glfwWindowShouldClose(handle)) {

		glfwGetWindowSize(handle, &width, &height);

		// Call update function

		updateFunc();

		// Render

		//glDrawPixels(width, height, GL_RGBA, GL_FLOAT, buffer);

		// Reset input

		for (uint k = 0; k < GLFW_KEY_LAST; k++) {
			keyPressed[k] = false;
			keyReleased[k] = false;
		}
		for (uint m = 0; m < GLFW_MOUSE_BUTTON_LAST; m++) {
			mousePressed[m] = false;
			mouseReleased[m] = false;
		}
		w->mouseMove = { 0, 0 };

		// Update FPS

		frame++;
		if (lastTime != (int)glfwGetTime()) {
			fps = frame;
			frame = 0;
		}

		lastTime = (int)glfwGetTime();

		glfwSwapBuffers(handle);
		glfwPollEvents();
	}

	glfwTerminate();

}

void Window::setTitle(string title) {

	glfwSetWindowTitle(handle, &title[0]);

}