#include "graphic.h"

Graphic::Graphic(GLuint texture) {

	this->texture = texture;

}

void Graphic::addVertex(Vec2 pos, Vec2 texCoord, Color color) {

	posData.push_back(Vec3(pos.x(), pos.y(), 0));
	texCoordData.push_back(texCoord);
	colorData.push_back(color);

}

Quad::Quad(GLuint texture, Vec2 pos, float width, float height, Color color) : Graphic(texture) {
	addVertex({ pos.x(), pos.y() }, { 0, 0 }, color);
	addVertex({ pos.x(), pos.y() + height }, { 0, 1 }, color);
	addVertex({ pos.x() + width, pos.y() }, { 1, 0 }, color);
	addVertex({ pos.x() + width, pos.y() }, { 1, 0 }, color);
	addVertex({ pos.x(), pos.y() + height }, { 0, 1 }, color);
	addVertex({ pos.x() + width, pos.y() + height }, { 1, 1 }, color);
}