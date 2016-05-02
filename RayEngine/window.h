#pragma once

#include "common.h"

// Window class for handling input and the program loop.
struct Window {

	void init(int width, int height);
	void open(function<void()> loopFunc, function<void()> resizeFunc);
	void setTitle(string title);

	int width, height, fps;
	float ratio;
	Vec2 mouse, mousePrevious, mouseMove;
	bool keyDown[GLFW_KEY_LAST], keyPressed[GLFW_KEY_LAST], keyReleased[GLFW_KEY_LAST];
	bool mouseDown[GLFW_MOUSE_BUTTON_LAST], mousePressed[GLFW_MOUSE_BUTTON_LAST], mouseReleased[GLFW_MOUSE_BUTTON_LAST];

	GLFWwindow* handle;
	function<void()> resizeFunc;
	Mat4x4 ortho;

};
