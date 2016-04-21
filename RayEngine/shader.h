// A class for loading and using shaders.

#pragma once
#include "common.h"
#include "graphic.h"
#include "triangle_mesh.h"

struct Shader {

	// Load shaders from text files.
	Shader(string name, function<void(GLuint, void*)> setup, string vertexFilename, string fragmentFilename, string geometryFilename = "");

	// Renders a 2D graphic.
	void use(Graphic* graphic, Mat4x4 matrix);
	void use(Graphic* graphic, Mat4x4 matrix, void* caller);

	// Render a 3D mesh.
	void use(TriangleMesh* mesh, Mat4x4 matrix);
	void use(TriangleMesh* mesh, Mat4x4 matrix, void* caller);

	string name;
	GLuint program, vbo;
	function<void(GLuint, void*)> setup;
};
