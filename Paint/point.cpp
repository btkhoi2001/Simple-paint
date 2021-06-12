#include "point.h"

MyShape::Point::Point() = default;

MyShape::Point::Point(float x, float y) {
	this->x = x;
	this->y = y;
}

string MyShape::Point::toString() {
	stringstream writer;

	writer << x << "|";
	writer << y;

	return writer.str();
}

MyShape::Point MyShape::Point::parse(string buffer) {
	vector<string> tokens = Tokenizer::spilit(buffer, "|");
	float x = stof(tokens[0]);
	float y = stof(tokens[1]);
	Point point(x, y);

	return point;
}
