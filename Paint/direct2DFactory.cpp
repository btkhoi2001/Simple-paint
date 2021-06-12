#include "direct2DFactory.h"

CComPtr<ID2D1Factory> Direct2DFactory::_instance = NULL;

CComPtr<ID2D1Factory> Direct2DFactory::instance()
{
	if (!_instance) {
		D2D1_FACTORY_OPTIONS fo = {};
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, fo, &_instance);
	}

	return _instance;
}
