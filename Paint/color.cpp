#include "color.h"

MyShape::Color::Color() = default;

MyShape::Color::Color(float r, float g, float b, float a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

string MyShape::Color::toString() {
	stringstream writer;

	writer << r << "|";
	writer << g << "|";
	writer << b << "|";
	writer << a;

	return writer.str();
}

MyShape::Color MyShape::Color::parse(string buffer) {
	vector<string> tokens = Tokenizer::spilit(buffer, "|");
	float r = stof(tokens[0]);
	float g = stof(tokens[1]);
	float b = stof(tokens[2]);
	float a = stof(tokens[3]);
	Color color(r, g, b, a);

	return color;
}
