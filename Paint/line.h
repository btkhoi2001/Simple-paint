#pragma once
#include "shape.h"

namespace MyShape {
	class Line : public Shape {
	public:
		Line();
		void draw(ID2D1RenderTarget* pRenderTarget, CComPtr<ID2D1SolidColorBrush> pBrush);
		ShapeType getShapeType();
		shared_ptr<Shape> create();
		shared_ptr<Shape> parse(string buffer);
	};
}