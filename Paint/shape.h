#pragma once
#include "point.h"
#include "color.h"
#include "shapetype.h"
#include "direct2DFactory.h"

namespace MyShape {
	class Shape {
	protected:
		Point _topLeft;
		Point _bottomRight;
		Color _filledColor;
		Color _brushColor;
		FLOAT _strokeWidth;

	public:
		Shape();
		virtual void draw(ID2D1RenderTarget* pRenderTarget, CComPtr<ID2D1SolidColorBrush> pBrush) = 0;
		virtual ShapeType getShapeType() = 0;
		void setTopLeft(D2D1_POINT_2F topLeft);
		void setBottomRight(D2D1_POINT_2F bottomRight);
		void setFilledColor(D2D1_COLOR_F filledColor);
		void setBrushColor(D2D1_COLOR_F brushColor);
		void setStrokeWidth(FLOAT strokeWidth);
		string toString();
		virtual shared_ptr<Shape> create() = 0;
		virtual shared_ptr<Shape> parse(string buffer) = 0;
	};
}