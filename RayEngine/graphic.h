// Used for rendering dynamic 2D graphics.

#pragma once
#include "common.h"

struct Graphic {

	// Start creating a graphic with a texture.
	Graphic(GLuint texture);

	vector<Vec3> posData;
	vector<Vec2> texCoordData;
	vector<Color> colorData;
	GLuint texture;

	// Update the vertex buffer object.
	void updateBuffer();

	// Add a new vertex with a position, texture coordinate and color.
	void addVertex(Vec2 pos, Vec2 texCoord, Color color = { 255, 255, 255, 255 });

};

struct Quad : Graphic {
	Quad(GLuint texture, Vec2 pos, float width, float height, Color color);
};
