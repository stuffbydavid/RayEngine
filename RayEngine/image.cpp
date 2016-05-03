#include "image.h"

Image::Image(GLuint filter, string filename, string alphaFilename) :
    filter(filter)
{
	cout << "Loading image " << filename << "..." << endl;

	Magick::Image image;
	image.read(filename);
	Magick::PixelPacket* data = image.getPixels(0, 0, image.columns(), image.rows());
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

	// Load alpha map
	if (alphaFilename != "") {

		cout << "  Alpha map found: " << alphaFilename << endl;

		Magick::Image aImage;
		aImage.read(alphaFilename);
		Magick::PixelPacket* aData = aImage.getPixels(0, 0, aImage.columns(), aImage.rows());
		int aWidth = aImage.columns();
		int aHeight = aImage.rows();

		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				if (x >= aWidth || y >= aHeight)
					continue;
				int idest = x + y * width;
				int isrc = x + (aHeight - 1 - y) * aWidth;
				pixels[idest].a((float)aData[isrc].red / USHRT_MAX);
			}
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

	if (filter == GL_LINEAR) {
		float u = coord.x() * width - 0.5f;
		float v = coord.y() * height - 0.5f;
		int px = u;
		int py = v;
		float u_ratio = u - px;
		float v_ratio = v - py;
		float u_opposite = 1.f - u_ratio;
		float v_opposite = 1.f - v_ratio;
		return (getPixel(px, py) * u_opposite + getPixel(px + 1, py) * u_ratio) * v_opposite +
			   (getPixel(px, py + 1) * u_opposite + getPixel(px + 1, py + 1) * u_ratio) * v_ratio;
	} else
		return getPixel(coord.x() * width, coord.y() * height);

}

Color Image::getPixel(int x, int y) {

	int mx = mod(x, width);
	int my = mod(y, height);
	return pixels[mx + my * width];

}