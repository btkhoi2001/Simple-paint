#include "shapeFactory.h"

MyShape::ShapeFactory* MyShape::ShapeFactory::_instance = NULL;

MyShape::ShapeFactory::ShapeFactory() {
	registerShape(LINE, make_shared<Line>());
	registerShape(RECTANGLE, make_shared<Rectangle>());
	registerShape(TRIANGLE, make_shared<Triangle>());
	registerShape(ELLIPSE, make_shared<Ellipse>());
}

void MyShape::ShapeFactory::registerShape(ShapeType shapeType, shared_ptr<Shape> shape) {
	_factoryMap[shapeType] = shape;
}

MyShape::ShapeFactory* MyShape::ShapeFactory::instance() {
	if (_instance == NULL)
		_instance = new ShapeFactory();

	return _instance;
}

shared_ptr<MyShape::Shape> MyShape::ShapeFactory::createShape(ShapeType shapeType) {
	map<ShapeType, shared_ptr<Shape>>::iterator it = _factoryMap.find(shapeType);
	if (it != _factoryMap.end())
		return it->second->create();

	return NULL;
}

shared_ptr<MyShape::Shape> MyShape::ShapeFactory::createShape(ShapeType shapeType, string buffer) {
	map<ShapeType, shared_ptr<Shape>>::iterator it = _factoryMap.find(shapeType);
	if (it != _factoryMap.end())
		return it->second->parse(buffer);

	return NULL;
}
