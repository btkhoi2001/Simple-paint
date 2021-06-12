#include "dpiscale.h"

float DPIScale::_scaleX = 1.0f;
float DPIScale::_scaleY = 1.0f;

void DPIScale::initialize(HWND hwnd) {
	UINT dpi = GetDpiForWindow(hwnd);
	_scaleX = _scaleY = dpi / 96.0f;
}

float DPIScale::pixelToDipsX(float x) {
	return x / _scaleX;
}

float DPIScale::pixelToDipsY(float y) {
	return y / _scaleY;
}
