// Renders using a color
#version 420 core

in vec3 aPos;
uniform mat4 uMat;

void main(void) {
	gl_Position = uMat * vec4(aPos, 1.0);
}