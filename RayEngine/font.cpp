/*#include "font.h"

Font::Font(FT_Library* lib, string filename, uint start, uint end, uint size) {
	FT_Face face;

	this->start = start;
	this->end = end;

	cout << "Loading " << filename << "..." << endl;

	if (FT_New_Face(*lib, &filename[0], 0, &face)) {
		cout << "Could not open font!";
		return;
	}

	FT_GlyphSlot glyph = face->glyph;
	FT_Set_Pixel_Sizes(face, 0, size);

	// Create character list
	character = new charInfo[end];

	// Get map dimensions
	mapWidth = 0;
	mapHeight = 0;

	for (uint i = start; i < end; i++) {
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			cout << "Loading character " << (char)i << " failed!" << endl;
			continue;
		}

		character[i] = {
			(float)glyph->bitmap.width,
			(float)glyph->bitmap.rows,
			(float)glyph->bitmap_left,
			(float)glyph->bitmap_top,
			(float)glyph->advance.x / 64,
			(float)glyph->advance.y / 64,
			(float)mapWidth,
		};

		mapWidth += glyph->bitmap.width;
		mapHeight = max(mapHeight, (uint)glyph->bitmap.rows);

	}

	// Create map texture
	glGenTextures(1, &map);
	glBindTexture(GL_TEXTURE_2D, map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mapWidth, mapHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Draw bitmaps onto map
	for (uint i = start; i < end; i++) {
		charInfo curChar = character[i];

		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			cout << "Loading character " << (char)i << " failed!" << endl;
			continue;
		}

		// Convert buffer to RGBA
		Color* glyphBuf = new Color[(int)curChar.width * (int)curChar.height];
		for (uint j = 0; j < curChar.width * curChar.height; j++)
			glyphBuf[j] = Color(1.f, 1.f, 1.f, glyph->bitmap.buffer[j] / 255.f);

		glTexSubImage2D(GL_TEXTURE_2D, 0, curChar.mapX, 0, curChar.width, curChar.height, GL_RGBA, GL_FLOAT, glyphBuf);

		delete glyphBuf;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	cout << "map is " << mapWidth << "x" << mapHeight << endl << endl;
}

void Font::renderText(string text, int x, int y, Color color, Shader* shader) {
	Graphic textGraphic(map);
	int dx = x, dy = y;

	for (uint i = 0; i < text.size(); i++) {
		uchar curChar = text[i];

		if (curChar == '\n') {
			dx = x;
			dy += mapHeight * 1.25;
			continue;
		}

		if (curChar < start || curChar > end)
			continue;

		charInfo curCharInfo = character[curChar];

		if (curCharInfo.width && curCharInfo.height) {
			float vx = dx + curCharInfo.left;
			float vy = dy + mapHeight - curCharInfo.top;
			float vw = curCharInfo.width;
			float vh = curCharInfo.height;
			float tx = curCharInfo.mapX / mapWidth;
			float tw = curCharInfo.width / mapWidth;
			float th = curCharInfo.height / mapHeight;

			textGraphic.addVertex({ vx, vy }, { tx, 0 }, color);
			textGraphic.addVertex({ vx, vy + vh }, { tx, th }, color);
			textGraphic.addVertex({ vx + vw, vy }, { tx + tw, 0 }, color);
			textGraphic.addVertex({ vx + vw, vy }, { tx + tw, 0 }, color);
			textGraphic.addVertex({ vx, vy + vh }, { tx, th }, color);
			textGraphic.addVertex({ vx + vw, vy + vh }, { tx + tw, th }, color);
		}

		dx += curCharInfo.advanceX;
		dy += curCharInfo.advanceY;
	}

	//shader->use(&textGraphic, window->ortho);
}*/