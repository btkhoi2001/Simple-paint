#pragma once
#include "shape.h"

namespace MyShape {
	class Triangle : public Shape {
	private:
		CComPtr<ID2D1PathGeometry> getTriangle();

	public:
		Triangle();
		void draw(ID2D1RenderTarget* pRenderTarget, CComPtr<ID2D1SolidColorBrush> pBrush);
		ShapeType getShapeType();
		shared_ptr<Shape> create();
		shared_ptr<Shape> parse(string buffer);
	};
}