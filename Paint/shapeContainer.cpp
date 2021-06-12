#include "shapeContainer.h"

void MyShape::ShapeContainer::push(shared_ptr<Shape> shape) {
	_shapes.insert(_shapes.end(), shape);
}

void MyShape::ShapeContainer::pop() {
	_shapes.pop_back();
}

void MyShape::ShapeContainer::clear() {
	_shapes.clear();
}

void MyShape::ShapeContainer::draw(ID2D1RenderTarget* pRenderTarget, CComPtr<ID2D1SolidColorBrush> pBrush) {
	for (auto i = _shapes.begin(); i != _shapes.end(); ++i)
		(*i)->draw(pRenderTarget, pBrush);
}

void MyShape::ShapeContainer::saveToFile(wstring filePath) {
	ofstream ofs(filePath);

	ofs << _shapes.size() << endl;
	for (auto i = _shapes.begin(); i != _shapes.end(); ++i)
		ofs << (*i)->toString() << endl;

	ofs.close();
}

void MyShape::ShapeContainer::loadFromFile(wstring filePath) {
	_shapes.clear();
	ifstream ifs(filePath);
	string buffer;
	int size;

	getline(ifs, buffer);
	size = stoi(buffer);

	for (int i = 0; i < size; i++) {
		getline(ifs, buffer);
		vector<string> tokens = Tokenizer::spilit(buffer, "|||");
		shared_ptr<MyShape::Shape> shape = ShapeFactory::instance()->createShape((MyShape::ShapeType)stoi(tokens[0]), tokens[1]);
		_shapes.insert(_shapes.end(), shape);
	}

	ifs.close();
}
