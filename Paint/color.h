#pragma once
#include "direct2DFactory.h"
#include "tokenizer.h"
#include <string>
#include <sstream>
#include <vector>
using namespace std;

namespace MyShape {
	class Color : public D2D1_COLOR_F {
	public:
		Color();
		Color(float r, float g, float b, float a);
		string toString();
		static Color parse(string buffer);
	};
}