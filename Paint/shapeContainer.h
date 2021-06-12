#pragma once
#include "shapeFactory.h"
#include <list>
#include <memory>
#include <fstream>

namespace MyShape {
	class ShapeContainer {
	private:
		list<shared_ptr<Shape>> _shapes;

	public:
		void push(shared_ptr<Shape> shape);
		void pop();
		void clear();
		void draw(ID2D1RenderTarget* pRenderTarget, CComPtr<ID2D1SolidColorBrush> pBrush);
		void saveToFile(wstring filePath);
		void loadFromFile(wstring filePath);
	};
}