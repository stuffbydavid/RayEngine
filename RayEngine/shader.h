#pragma once
#include "common.h"
#include "graphic.h"
#include "triangle_mesh.h"

// Loads and renders using a vertex/fragment/geometry shader.
struct Shader {

	// Load shaders from text files.
	Shader(string name, function<void(GLuint, void*)> setup, string vertexFilename, string fragmentFilename, string geometryFilename = "");

	// Render a 3D mesh.
	void use(TriangleMesh* mesh, Mat4x4 matrix, void* caller = nullptr);

	// Renders a 2D graphic.
	//void use(Graphic* graphic, Mat4x4 matrix, void* caller = nullptr);

	static void setupOGL(GLuint program, void* caller);

	string name;
	GLuint program, vbo;
	function<void(GLuint, void*)> setup;
};
