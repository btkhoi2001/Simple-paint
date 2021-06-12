#include "shape.h"

MyShape::Shape::Shape() = default;

void MyShape::Shape::setTopLeft(D2D1_POINT_2F topLeft) {
	_topLeft.x = topLeft.x;
	_topLeft.y = topLeft.y;
}

void MyShape::Shape::setBottomRight(D2D1_POINT_2F bottomRight) {
	_bottomRight.x = bottomRight.x;
	_bottomRight.y = bottomRight.y;
}

void MyShape::Shape::setFilledColor(D2D1_COLOR_F filledColor) {
	_filledColor.r = filledColor.r;
	_filledColor.g = filledColor.g;
	_filledColor.b = filledColor.b;
	_filledColor.a = filledColor.a;
}

void MyShape::Shape::setBrushColor(D2D1_COLOR_F brushColor) {
	_brushColor.r = brushColor.r;
	_brushColor.g = brushColor.g;
	_brushColor.b = brushColor.b;
	_brushColor.a = brushColor.a;
}

void MyShape::Shape::setStrokeWidth(FLOAT strokeWidth) {
	_strokeWidth = strokeWidth;
}

string MyShape::Shape::toString() {
	stringstream writer;

	writer << getShapeType() << "|||";
	writer << _topLeft.toString() << "||";
	writer << _bottomRight.toString() << "||";
	writer << _filledColor.toString() << "||";
	writer << _brushColor.toString() << "||";
	writer << _strokeWidth;

	return writer.str();
}
