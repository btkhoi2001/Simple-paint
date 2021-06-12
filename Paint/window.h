#pragma once
#include "resource.h"
#include "shapefactory.h"
#include "shapeContainer.h"
#include "dpiscale.h"
#include <Windows.h>
#include <windowsx.h>
#include <ShObjIdl.h>
#include <deque>
#include <string>
#include <fstream>

class Window {
private:
	enum MouseMode {
		CROSS,
		ARROW
	};

	HWND _mainHwnd;
	HWND _toolbarHwnd;
	HWND _canvasHwnd;

	HIMAGELIST _hImageList;

	MouseMode _mouseMode;
	MyShape::ShapeType _shapeType;
	D2D1_COLOR_F _filledColor;
	D2D1_COLOR_F _brushColor;
	FLOAT _strokeWidth;

	MyShape::ShapeContainer _shapeContainer;
	shared_ptr<MyShape::Shape> _selection;
	deque<shared_ptr<MyShape::Shape>> _undo;
	deque<shared_ptr<MyShape::Shape>> _redo;

	HCURSOR _hCursor;
	CComPtr<ID2D1HwndRenderTarget> _pRenderTarget;
	CComPtr<ID2D1SolidColorBrush> _pBrush;
	D2D1_POINT_2F _ptMouse;

	COMDLG_FILTERSPEC _fileTypes[1];
	wstring _filePath;
	bool _isSaved;
	bool _isBinded;

	static LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK canvasWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT mainHandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT canvasHandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void onCreate();
	void onClose();
	void onResize();
	void onLButtonDown(int pixelX, int pixelY, DWORD flags);
	void onLButtonUp();
	void onMouseMove(int pixelX, int pixelY, DWORD flags);
	void onNotify(LPARAM lParam);
	void onCommand(WORD id);
	void onPaint();
	void onSetCursor(MouseMode mode);
	void changeShapeType(MyShape::ShapeType shapeType);
	void changeStrokeWidth(float width);
	void chooseColor(D2D1_COLOR_F& color);

	void resetWindow();
	int showSaveConfirmDialog();
	void createNewObject();
	HRESULT showOpenObjectDialog();
	HRESULT showSaveObjectDialog();
	HRESULT showSaveAsObjectDialog();
	void undoLastAction();
	void redoLastAction();

	HRESULT insertShape(float x, float y);
	HRESULT createGraphicResources();

	HWND createToolbar();
	HWND createCanvas();

public:
	Window();
	BOOL create(
		PCWSTR lpWindownName,
		DWORD dwStyle,
		DWORD dwExStyle = 0,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		HWND hWndParent = 0,
		HMENU hMenu = 0);
	HWND window();
};