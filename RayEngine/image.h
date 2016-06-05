#pragma once

#include "util.h"
#include "vec2.h"
#include "color.h"

#include "GL/glew.h"
#include <Magick++.h>

struct Image {

	// Create an image from a file.
	Image(GLuint filter, string filename, string alphaFilename = "");

	// Create an image from a single color.
	Image(Color color);

	// Create an image from a buffer
	Image(Color* pixels, int width, int height, GLuint filter);

	// Creates an OpenGL texture object.
	void createTexture();

	// Gets the color of a pixel.
	Color getPixel(Vec2 coord);
	Color getPixel(int x, int y);

	// Variables
	Color *pixels;
	int width, height;
	GLuint texture;
	GLuint filter;

};