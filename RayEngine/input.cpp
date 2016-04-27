#include "rayengine.h"

void RayEngine::input() {

	static bool moveCamera = false;
	Camera& selectedCamera = scenes[0]->camera;

	// Start/Stop moving

	if (window.mousePressed[GLFW_MOUSE_BUTTON_LEFT]) {
		moveCamera = true;
		glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (window.mouseReleased[GLFW_MOUSE_BUTTON_LEFT]) {
		moveCamera = false;
		glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// Render mode

	if (window.keyPressed[GLFW_KEY_F1]) {
		renderMode = RM_OPENGL;
	}

	if (window.keyPressed[GLFW_KEY_F2]) {
		renderMode = RM_RAYTRACING;
		rayTracingTarget = RTT_CPU;
	}

	if (window.keyPressed[GLFW_KEY_F3]) {
		renderMode = RM_RAYTRACING;
		rayTracingTarget = RTT_GPU;
	}

	if (window.keyPressed[GLFW_KEY_F4]) {
		renderMode = RM_RAYTRACING;
		rayTracingTarget = RTT_HYBRID;
	}

	// Move the camera

	if (moveCamera) {

		// TODO: This should depend on framerate
		float lookFactor = 0.2f;
		float moveFactor = 0.2f;
		float rotateFactor = 0.2f;

		// Rotate X and Z axis around Y axis for yaw

		selectedCamera.xaxis = Mat4x3::rotate(selectedCamera.xaxis, selectedCamera.yaxis, -window.mouseMove.x() * lookFactor);
		selectedCamera.zaxis = Mat4x3::rotate(selectedCamera.zaxis, selectedCamera.yaxis, -window.mouseMove.x() * lookFactor);

		// Rotate Y and Z axis around X axis for pitch

		selectedCamera.yaxis = Mat4x3::rotate(selectedCamera.yaxis, selectedCamera.xaxis, -window.mouseMove.y() * lookFactor);
		selectedCamera.zaxis = Mat4x3::rotate(selectedCamera.zaxis, selectedCamera.xaxis, -window.mouseMove.y() * lookFactor);

		// Go faster

		if (window.keyDown[GLFW_KEY_SPACE])
			moveFactor *= 10;

		// Go slower

		if (window.keyDown[GLFW_KEY_LEFT_SHIFT])
			moveFactor /= 10;

		// Move along the axises

		if (window.keyDown[GLFW_KEY_W])
			selectedCamera.position += selectedCamera.zaxis * moveFactor;

		if (window.keyDown[GLFW_KEY_S])
			selectedCamera.position -= selectedCamera.zaxis * moveFactor;

		if (window.keyDown[GLFW_KEY_D])
			selectedCamera.position += selectedCamera.xaxis * moveFactor;

		if (window.keyDown[GLFW_KEY_A])
			selectedCamera.position -= selectedCamera.xaxis * moveFactor;

		// Roll (rotate X and Y axis around Z axis)

		if (window.keyDown[GLFW_KEY_Q]) {
			selectedCamera.xaxis = Mat4x3::rotate(selectedCamera.xaxis, selectedCamera.zaxis, -rotateFactor);
			selectedCamera.yaxis = Mat4x3::rotate(selectedCamera.yaxis, selectedCamera.zaxis, -rotateFactor);
		}

		if (window.keyDown[GLFW_KEY_E]) {
			selectedCamera.xaxis = Mat4x3::rotate(selectedCamera.xaxis, selectedCamera.zaxis, rotateFactor);
			selectedCamera.yaxis = Mat4x3::rotate(selectedCamera.yaxis, selectedCamera.zaxis, rotateFactor);
		}

		// FOV

		if (window.keyDown[GLFW_KEY_T])
			selectedCamera.fov++;

		if (window.keyDown[GLFW_KEY_G])
			selectedCamera.fov--;
	}
}