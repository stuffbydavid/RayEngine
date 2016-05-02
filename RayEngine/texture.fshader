#version 420 core

in vec2 vTexCoord;
out vec4 fColor;
uniform sampler2D uSampler;
uniform vec4 uColor;

void main(void) {
	fColor = uColor * texture2D(uSampler, vTexCoord);
}