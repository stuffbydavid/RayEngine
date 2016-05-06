#include "rayengine.h"

void glUniformVec3(GLint location, Vec3 vec) {
	glUniform3f(location, vec.x(), vec.y(), vec.z());
}

void glUniformColor(GLint location, Color color) {
	glUniform4f(location, color.r(), color.g(), color.b(), color.a());
}

void RayEngine::openglSetupNormals(GLuint program, Object* object, TriangleMesh* mesh) {

	GLint uMatWorld = glGetUniformLocation(program, "uMatWorld");
	glUniformMatrix4fv(uMatWorld, 1, GL_FALSE, object->matrix.e);

}

void RayEngine::openglSetupTexture(GLuint program, Object* object, TriangleMesh* mesh) {

	GLint uColor = glGetUniformLocation(program, "uColor");
	glUniformColor(program, { 1.f, 1.f, 1.f });

}

void RayEngine::openglSetupPhong(GLuint program, Object* object, TriangleMesh* mesh) {

	uint lightCount = curScene->lights.size();

	GLint uMatWorld = glGetUniformLocation(program, "uMatWorld");
	GLint uEyePos = glGetUniformLocation(program, "uEyePos");
	GLint uLights = glGetUniformLocation(program, "uLights");
	GLint uLightPos = glGetUniformLocation(program, "uLightPos");
	GLint uLightColor = glGetUniformLocation(program, "uLightColor");
	GLint uLightRange = glGetUniformLocation(program, "uLightRange");
	GLint uAmbient = glGetUniformLocation(program, "uAmbient");
	GLint uSpecular = glGetUniformLocation(program, "uSpecular");
	GLint uDiffuse = glGetUniformLocation(program, "uDiffuse");
	GLint uShininess = glGetUniformLocation(program, "uShininess");

	glUniformMatrix4fv(uMatWorld, 1, GL_FALSE, object->matrix.e);
	glUniformVec3(uEyePos, curCamera->position);

	// Create light arrays
	Vec3* lightPos = new Vec3[lightCount];
	Color* lightColor = new Color[lightCount];
	float* lightRange = new float[lightCount];
	for (uint i = 0; i < lightCount; i++) {
		lightPos[i] = curScene->lights[i].position;
		lightColor[i] = curScene->lights[i].color;
		lightRange[i] = curScene->lights[i].range;
	}

	glUniform1f(uLights, lightCount);
	glUniform3fv(uLightPos, lightCount, (float*)lightPos);
	glUniform4fv(uLightColor, lightCount, (float*)lightColor);
	glUniform1fv(uLightRange, lightCount, lightRange);

	glUniformColor(uAmbient, curScene->ambient + mesh->material->ambient);
	glUniformColor(uSpecular, mesh->material->specular);
	glUniformColor(uDiffuse, mesh->material->diffuse);
	glUniform1f(uShininess, mesh->material->shininess);

	delete lightPos;
	delete lightColor;
	delete lightRange;

}