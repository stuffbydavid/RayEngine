// Displays the normals as colors
#version 420 core

in vec3 aPos;
in vec3 aNormal;
in vec2 aTexCoord;
out vec4 vColor;
uniform mat4 uMat;

void main(void) {
	vColor = vec4(aNormal * 0.5 +0.5, 1.0);
	gl_Position = uMat * vec4(aPos, 1.0);
}