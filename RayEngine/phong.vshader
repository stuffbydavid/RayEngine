// Phong shading
#version 420 core

in vec3 aPos;
in vec3 aNormal;
in vec2 aTexCoord;
out vec2 vTexCoord;
out vec3 vPosWorld;
out vec3 vNormalWorld;
uniform mat4 uMat;
uniform mat4 uMatWorld;

void main(void) {
	vPosWorld = vec3(uMatWorld * vec4(aPos, 1.0));
	vNormalWorld = normalize(vec3(uMatWorld * vec4(aNormal, 0.0)));
	vTexCoord = aTexCoord;
	gl_Position = uMat * vec4(aPos, 1.0);
}