#pragma once
#include "tokenizer.h"
#include "direct2DFactory.h"
#include <string>
#include <sstream>
#include <vector>
using namespace std;

namespace MyShape {
	class Point : public D2D1_POINT_2F {
	public:
		Point();
		Point(float x, float y);
		string toString();
		static Point parse(string buffer);
	};
}