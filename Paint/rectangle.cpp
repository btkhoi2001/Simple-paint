#include "rectangle.h"

D2D1_RECT_F MyShape::Rectangle::getRectangle() {
	return D2D1::RectF(_topLeft.x, _topLeft.y, _bottomRight.x, _bottomRight.y);
}

MyShape::Rectangle::Rectangle() = default;

void MyShape::Rectangle::draw(ID2D1RenderTarget* pRenderTarget, CComPtr<ID2D1SolidColorBrush> pBrush) {
	D2D1_RECT_F rectangle = getRectangle();
	pBrush->SetColor(_filledColor);
	pRenderTarget->FillRectangle(rectangle, pBrush);
	pBrush->SetColor(_brushColor);
	pRenderTarget->DrawRectangle(rectangle, pBrush, _strokeWidth);
}

MyShape::ShapeType MyShape::Rectangle::getShapeType() {
	return RECTANGLE;
}

shared_ptr<MyShape::Shape> MyShape::Rectangle::create() {
	return make_shared<MyShape::Rectangle>();
}

shared_ptr<MyShape::Shape> MyShape::Rectangle::parse(string buffer) {
	shared_ptr<Rectangle> rectangle =  make_shared<Rectangle>();
	vector<string> tokens = Tokenizer::spilit(buffer, "||");

	Point topLeft = Point::parse(tokens[0]);
	Point bottomRight = Point::parse(tokens[1]);
	Color filledColor = Color::parse(tokens[2]);
	Color brushColor = Color::parse(tokens[3]);
	FLOAT strokeWidth = stof(tokens[4]);

	rectangle->setTopLeft(topLeft);
	rectangle->setBottomRight(bottomRight);
	rectangle->setFilledColor(filledColor);
	rectangle->setBrushColor(brushColor);
	rectangle->setStrokeWidth(strokeWidth);

	return rectangle;
}
