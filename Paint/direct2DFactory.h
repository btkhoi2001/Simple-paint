#pragma once
#include <d2d1.h>
#include <CommCtrl.h>
#include <atlbase.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "d2d1")

class Direct2DFactory {
private:
	static CComPtr<ID2D1Factory> _instance;

public:
	static CComPtr<ID2D1Factory> instance();
};