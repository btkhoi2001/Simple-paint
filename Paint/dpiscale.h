#pragma once
#include <Windows.h>
#include <windowsX.h>

class DPIScale {
private:
	static float _scaleX;
	static float _scaleY;

public:
	static void initialize(HWND hwnd);
	static float pixelToDipsX(float x);
	static float pixelToDipsY(float y);
};

