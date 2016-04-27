#version 420 core

in vec2 vTexCoord;
out vec4 fColor;
uniform sampler2D uTex;
uniform vec4 uColor;

void main(void) {
	fColor = texture2D(uTex, vTexCoord) * uColor;
}