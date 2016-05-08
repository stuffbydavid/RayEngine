#version 420 core

out vec4 fColor;
uniform vec4 uColor;

void main(void) {
	fColor = uColor;
}