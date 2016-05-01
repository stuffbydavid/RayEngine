#include "common.h"

struct Image {

	// Create an image from a file.
	Image(string filename, GLuint filter);

	// Create an image from a single color.
	Image(Color color);

	// Creates an OpenGL texture object.
	void createTexture();

	// Gets the color of a pixel.
	Color getPixel(Vec2 coord);

	// Variables
	Color *pixels;
	int width, height;
	GLuint texture;
	GLuint filter;

};