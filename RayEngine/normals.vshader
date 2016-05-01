// Displays the normals as colors
#version 420 core

in vec3 aPos;
in vec3 aNormal;
in vec2 aTexCoord;
out vec4 vColor;
uniform mat4 uMat;
uniform mat4 uMatWorld;

void main(void) {
	vec3 vNormalWorld = normalize(vec3(uMatWorld * vec4(aNormal, 0.0)));
	vColor = vec4(vNormalWorld * 0.5 +0.5, 1.0);
	gl_Position = uMat * vec4(aPos, 1.0);
}