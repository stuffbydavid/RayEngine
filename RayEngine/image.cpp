#include "image.h"

Image::Image(string filename, GLuint filter) :
    filter(filter)
{
	cout << "Loading image " << filename << "..." << endl;

	Magick::Image image;
	Magick::PixelPacket* data;
	image.read(filename); 
	data = image.getPixels(0, 0, image.columns(), image.rows());
	width = image.columns();
	height = image.rows();

	cout << "  Width: " << width << endl;
	cout << "  Height: " << height << endl;

	// Convert to floats and flip vertically
	pixels = new Color[width * height];
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			int idest = x + y * width;
			int isrc = x + (height - 1 - y) * width;
			pixels[idest] = {
				(float)data[isrc].red / USHRT_MAX,
				(float)data[isrc].green / USHRT_MAX,
				(float)data[isrc].blue / USHRT_MAX,
				1.f - (float)data[isrc].opacity / USHRT_MAX
			};
		}
	}
	createTexture();

}

Image::Image(Color color) {

	width = height = 1;
	pixels = new Color[1];
	pixels[0] = color;
	filter = GL_NEAREST;
	createTexture();

}


void Image::createTexture() {

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

}

long mod(long a, long b) {
	return (a % b + b) % b;
}

Color Image::getPixel(Vec2 coord) {

	int x = mod(coord.x() * width, width);
	int y = mod(coord.y() * height, height);

	return pixels[x + y * width];

}