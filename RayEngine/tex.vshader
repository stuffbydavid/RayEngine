// Renders using a colored texture
#version 420 core

in vec3 aPos;
in vec2 aTexCoord;
out vec2 vTexCoord;
uniform mat4 uMat;

void main(void) {
	vTexCoord = aTexCoord;
	gl_Position = uMat * vec4(aPos, 1.0);
}