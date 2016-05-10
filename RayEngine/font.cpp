#include "font.h"

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
	chars = new CharInfo[end];

	// Get map dimensions
	mapWidth = 0;
	mapHeight = 0;

	for (uint i = start; i < end; i++) {
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			cout << "Loading character " << (char)i << " failed!" << endl;
			continue;
		}

		chars[i] = {
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
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mapWidth, mapHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Draw bitmaps onto map
	for (uint i = start; i < end; i++) {
		CharInfo curChar = chars[i];

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

void Font::renderText(string text, int x, int y, Color color, Shader* shader, Mat4x4 matrix) {

	uint textLength = text.size();
	Vec3* posData = new Vec3[textLength * 6];
	Vec2* texCoordData = new Vec2[textLength * 6];

	int dx = x, dy = y;

	for (uint c = 0; c < textLength; c++) {

		uchar curChar = text[c];

		if (curChar == '\n') {
			dx = x;
			dy += mapHeight * 1.25;
			continue;
		}

		if (curChar < start || curChar > end)
			continue;

		CharInfo curCharInfo = chars[curChar];

		if (curCharInfo.width && curCharInfo.height) {

			float vx = dx + curCharInfo.left;
			float vy = dy + mapHeight - curCharInfo.top;
			float vw = curCharInfo.width;
			float vh = curCharInfo.height;
			float tx = curCharInfo.mapX / mapWidth;
			float tw = curCharInfo.width / mapWidth;
			float th = curCharInfo.height / mapHeight;

			int i = c * 6;

			posData[i + 0] = { vx, vy, 0.f };
			posData[i + 1] = { vx, vy + vh, 0.f };
			posData[i + 2] = { vx + vw, vy, 0.f };
			posData[i + 3] = { vx + vw, vy, 0.f };
			posData[i + 4] = { vx, vy + vh, 0.f };
			posData[i + 5] = { vx + vw, vy + vh, 0.f };

			texCoordData[i + 0] = { tx, 0.f };
			texCoordData[i + 1] = { tx, th };
			texCoordData[i + 2] = { tx + tw, 0.f };
			texCoordData[i + 3] = { tx + tw, 0.f };
			texCoordData[i + 4] = { tx, th };
			texCoordData[i + 5] = { tx + tw, th };

		}

		dx += curCharInfo.advanceX;
		dy += curCharInfo.advanceY;

	}
	
	shader->render2D(matrix, posData, texCoordData, textLength * 6, texture, color);

	delete posData;
	delete texCoordData;

}