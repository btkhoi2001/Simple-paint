#pragma once
#include "shape.h"
#include "line.h"
#include "rectangle.h"
#include "ellipse.h"
#include "triangle.h"
#include <memory>
#include <map>
#include <vector>
using namespace std;

namespace MyShape {
	class ShapeFactory {
	private:
		static ShapeFactory* _instance;
		map<ShapeType, shared_ptr<Shape>> _factoryMap;
		ShapeFactory();
		void registerShape(ShapeType shapeType, shared_ptr<Shape> shape);

	public:
		static ShapeFactory* instance();
		shared_ptr<Shape> createShape(ShapeType shapeType);
		shared_ptr<Shape> createShape(ShapeType shapeType, string buffer);
	};
}