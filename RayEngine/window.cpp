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

	// The window was resized!

	width = max(1, width);
	height = max(1, height);

	glViewport(0, 0, width, height);
	w->ortho = Mat4x4::ortho(0, width, 0, height, 0, 1);
	w->width = width;
	w->height = height;
	w->ratio = (float)width / height;
	w->resizeFunc();

}

void Window::init(int width, int height) {

	// Initialize window

	glfwInit();
	handle = glfwCreateWindow(width, height, "", NULL, NULL);
	glfwMakeContextCurrent(handle);
	this->width = width;
	this->height = height;
	w = this;

	// Init GLEW

	glewExperimental = true;
	glewInit();

	cout << "GLEW version: " << glewGetString(GLEW_VERSION) << endl;
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "OpenGL SL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << endl;
	cout << endl;

	// Culling

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	// Transparent blending

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Texture settings

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Initialize input

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
	
}

void Window::open(function<void(void)> loopFunc, function<void(void)> resizeFunc) {

	cout << "Opening window..." << endl;

	int lastTime = -1, frame = 0;
	fps = 0;

	// Set callbacks

	this->resizeFunc = resizeFunc;
	glfwSetMouseButtonCallback(handle, windowMouseButtonCallback);
	glfwSetCursorPosCallback(handle, windowCursorPosCallback);
	glfwSetKeyCallback(handle, windowKeyCallback);
	glfwSetWindowSizeCallback(handle, windowSizeCallback);
	windowSizeCallback(handle, width, height);

	while (!glfwWindowShouldClose(handle)) {

		// Call loop function

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		loopFunc();

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

		// Swap buffers

		glfwSwapBuffers(handle);
		glfwPollEvents();

	}

	glfwTerminate();

}

void Window::setTitle(string title) {

	glfwSetWindowTitle(handle, &title[0]);

}