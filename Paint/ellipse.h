#pragma once
#include "shape.h"

namespace MyShape {
	class Ellipse : public Shape {
	private:
		D2D1_ELLIPSE getEllipse();

	public:
		Ellipse();
		void draw(ID2D1RenderTarget* pRenderTarget, CComPtr<ID2D1SolidColorBrush> pBrush);
		ShapeType getShapeType();
		shared_ptr<Shape> create();
		shared_ptr<Shape> parse(string buffer);
	};
}