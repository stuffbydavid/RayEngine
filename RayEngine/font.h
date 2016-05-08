#pragma once

#include "common.h"
#include "shader.h"

struct CharInfo {
	float width, height, left, top, advanceX, advanceY, mapX;
};

// Font class for storing character maps and rendering text.
struct Font {

	// Load a new font from a file.
	Font(FT_Library* lib, string filename, uint start, uint end, uint size);

	// Renders a piece of text using the font.
	void Font::renderText(string text, int x, int y, Color color, Shader* shader);

	uint start, end, mapWidth, mapHeight;
	uint texture;
	CharInfo* chars;

};