#pragma once

#include "util.h"
#include "mat4x4.h"
#include "shader.h"

#include "ft2build.h"
#include FT_FREETYPE_H

struct CharInfo {
	float width, height, left, top, advanceX, advanceY, mapX;
};

// Font class for storing character maps and rendering text.
struct Font {

	// Load a new font from a file.
	Font(FT_Library* lib, string filename, uint start, uint end, uint size);

	// Renders a piece of text using the font.
	void Font::renderText(Shader* shader, Mat4x4 matrix, string text, int x, int y, Color color);

	uint start, end, width, height;
	uint texture;
	CharInfo* chars;

};