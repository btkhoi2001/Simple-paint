#include "line.h"

MyShape::Line::Line() = default;

void MyShape::Line::draw(ID2D1RenderTarget* pRenderTarget, CComPtr<ID2D1SolidColorBrush> pBrush) {
	pBrush->SetColor(_brushColor);
	pRenderTarget->DrawLine(_topLeft, _bottomRight, pBrush, _strokeWidth);
}

MyShape::ShapeType MyShape::Line::getShapeType() {
	return LINE;
}

shared_ptr<MyShape::Shape> MyShape::Line::create() {
	return make_shared<MyShape::Line>();
}

shared_ptr<MyShape::Shape> MyShape::Line::parse(string buffer) {
	shared_ptr<Line> line = make_shared<Line>();
	vector<string> tokens = Tokenizer::spilit(buffer, "||");

	Point topLeft = Point::parse(tokens[0]);
	Point bottomRight = Point::parse(tokens[1]);
	Color filledColor = Color::parse(tokens[2]);
	Color brushColor = Color::parse(tokens[3]);
	FLOAT strokeWidth = stof(tokens[4]);

	line->setTopLeft(topLeft);
	line->setBottomRight(bottomRight);
	line->setFilledColor(filledColor);
	line->setBrushColor(brushColor);
	line->setStrokeWidth(strokeWidth);

	return line;
}
