#pragma once

#include "common.h"
#include "object.h"

// Loads and renders using a vertex/fragment/geometry shader.
struct Shader {

	// Load shaders from text files.
	Shader(string name, function<void(GLuint , Object*, TriangleMesh*)> setup, string vertexFilename, string fragmentFilename, string geometryFilename = "");

	// Render a 3D mesh.
	void use(Mat4x4 matrix, Object* object, TriangleMesh* mesh);

	// Renders a 2D graphic.
	void use(Mat4x4 matrix, Vec3* posData, Vec2* texCoordData, int vertices, GLuint texture = 0, Color color = { 1.f });
	void use(Mat4x4 matrix, int x, int y, int width, int height, GLuint texture = 0, Color color = { 1.f });

	// Variables
	string name;
	GLuint program, vbo;
	function<void(GLuint, Object*, TriangleMesh*)> setup;

};
