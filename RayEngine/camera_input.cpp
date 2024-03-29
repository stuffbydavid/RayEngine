#include "rayengine.h"

void RayEngine::cameraInput() {

	static bool moveCamera = false;

	// Start/Stop moving

	if (window.mousePressed[GLFW_MOUSE_BUTTON_LEFT]) {
		moveCamera = true;
		glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (window.mouseReleased[GLFW_MOUSE_BUTTON_LEFT]) {
		moveCamera = false;
		glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// Move the camera

	if (moveCamera) {

		float lookFactor = 0.2f;
		float moveFactor = 2.f;
		float rotateFactor = 1.f;

		// Rotate X and Z axis around Y axis for yaw

		curCamera->xaxis = Vec3::rotate(curCamera->xaxis, curCamera->yaxis, -window.mouseMove.x() * lookFactor);
		curCamera->zaxis = Vec3::rotate(curCamera->zaxis, curCamera->yaxis, -window.mouseMove.x() * lookFactor);

		// Rotate Y and Z axis around X axis for pitch

		curCamera->yaxis = Vec3::rotate(curCamera->yaxis, curCamera->xaxis, -window.mouseMove.y() * lookFactor);
		curCamera->zaxis = Vec3::rotate(curCamera->zaxis, curCamera->xaxis, -window.mouseMove.y() * lookFactor);

		// Go faster

		if (window.keyDown[GLFW_KEY_SPACE])
			moveFactor *= 10;

		// Go slower

		if (window.keyDown[GLFW_KEY_LEFT_SHIFT])
			moveFactor /= 10;

		// Move along the axises

		if (window.keyDown[GLFW_KEY_W])
			curCamera->position += curCamera->zaxis * moveFactor;

		if (window.keyDown[GLFW_KEY_S])
			curCamera->position -= curCamera->zaxis * moveFactor;

		if (window.keyDown[GLFW_KEY_D])
			curCamera->position += curCamera->xaxis * moveFactor;

		if (window.keyDown[GLFW_KEY_A])
			curCamera->position -= curCamera->xaxis * moveFactor;

		// Roll (rotate X and Y axis around Z axis)

		if (window.keyDown[GLFW_KEY_Q]) {
			curCamera->xaxis = Vec3::rotate(curCamera->xaxis, curCamera->zaxis, -rotateFactor);
			curCamera->yaxis = Vec3::rotate(curCamera->yaxis, curCamera->zaxis, -rotateFactor);
		}

		if (window.keyDown[GLFW_KEY_E]) {
			curCamera->xaxis = Vec3::rotate(curCamera->xaxis, curCamera->zaxis, rotateFactor);
			curCamera->yaxis = Vec3::rotate(curCamera->yaxis, curCamera->zaxis, rotateFactor);
		}

		// FOV

		if (window.keyDown[GLFW_KEY_T])
			curCamera->setFov(curCamera->fov + 1.f);

		if (window.keyDown[GLFW_KEY_G])
			curCamera->setFov(curCamera->fov - 1.f);

	}

}