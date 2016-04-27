#pragma once

#include "common.h"

// Used for rendering dynamic 2D graphics.
struct Graphic {

	// Start creating a graphic with a texture.
	Graphic(GLuint texture);

	// Update the vertex buffer object.
	void updateBuffer();

	// Add a new vertex with a position, texture coordinate and color.
	void addVertex(Vec2 pos, Vec2 texCoord, Color color = { 1.f, 1.f, 1.f, 1.f });

	vector<Vec3> posData;
	vector<Vec2> texCoordData;
	vector<Color> colorData;
	GLuint texture;

};

struct Quad : Graphic {
	Quad(GLuint texture, Vec2 pos, float width, float height, Color color);
};
