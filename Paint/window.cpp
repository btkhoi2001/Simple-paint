#include "window.h"

LRESULT Window::mainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Window* pThis = NULL;

    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (Window*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->_mainHwnd = hwnd;
    }
    else {
        pThis = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        return pThis->mainHandleMessage(uMsg, wParam, lParam);
    }
    else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT Window::canvasWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Window* pThis = NULL;

    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (Window*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    }
    else {
        pThis = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        return pThis->canvasHandleMessage(uMsg, wParam, lParam);
    }
    else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT Window::mainHandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)  {
    switch (uMsg) {
        case WM_CREATE:
            onCreate();
            return 0;

        case WM_CLOSE:
            onClose();
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            onResize();
            return 0;

        case WM_NOTIFY:
            onNotify(lParam);
            return 0;

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                onSetCursor(ARROW);
                return 0;
            }
            break;

        case WM_COMMAND:
            onCommand(LOWORD(wParam));
            return 0;
    }

    return DefWindowProc(_mainHwnd, uMsg, wParam, lParam);
}

LRESULT Window::canvasHandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_LBUTTONDOWN:
            onLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
            return 0;

        case WM_LBUTTONUP:
            onLButtonUp();
            return 0;

        case WM_MOUSEMOVE:
            onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
            return 0;

        case WM_PAINT:
            onPaint();
            return 0;

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                onSetCursor(CROSS);
                return 0;
            }
            break;
    }

    return DefWindowProc(_canvasHwnd, uMsg, wParam, lParam);
}

void Window::onCreate() {
    _toolbarHwnd = createToolbar();
    _canvasHwnd = createCanvas();
    DPIScale::initialize(_mainHwnd);
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
}

void Window::onClose() {
    if (!_isSaved) {
        int msgboxID = showSaveConfirmDialog();

        switch (msgboxID) {
        case IDYES:
            showSaveObjectDialog();
            break;

        case IDNO:
            // Do nothing
            break;

        case IDCANCEL:
            // Do nothing
            return;
        }
    }

    DestroyWindow(_mainHwnd);
}

void Window::onResize() {
    SendMessage(_toolbarHwnd, TB_AUTOSIZE, 0, 0);

    if (_pRenderTarget != NULL) {
        RECT rcMain, rcToolbar;
        GetClientRect(_mainHwnd, &rcMain);
        GetClientRect(_toolbarHwnd, &rcToolbar);
        MoveWindow(_canvasHwnd, 0, rcToolbar.bottom, rcMain.right, rcMain.bottom - rcToolbar.bottom, TRUE);
        
        D2D1_SIZE_U size = D2D1::SizeU(rcMain.right, rcMain.bottom - rcToolbar.bottom);

        _pRenderTarget->Resize(size);

        InvalidateRect(_canvasHwnd, NULL, FALSE);
    }
}

void Window::onLButtonDown(int pixelX, int pixelY, DWORD flags) {
    float dipX = DPIScale::pixelToDipsX(pixelX);
    float dipY = DPIScale::pixelToDipsY(pixelY);
    POINT pt = { pixelX, pixelY };

    if (DragDetect(_canvasHwnd, pt)) {
        SetCapture(_canvasHwnd);
        insertShape(dipX, dipY);
    }

    InvalidateRect(_canvasHwnd, NULL, FALSE);
}

void Window::onLButtonUp() {
    if (_selection) {
        if (_undo.empty())
            SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_UNDO, (LPARAM)TBSTATE_ENABLED);

        if (!_redo.empty()) {
            SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_REDO, (LPARAM)0);
            _redo.clear();
        }


        _shapeContainer.push(_selection);
        _undo.push_back(_selection);    
        _selection = NULL;
        _isSaved = false;

        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_REDO, (LPARAM)0);
        InvalidateRect(_canvasHwnd, NULL, FALSE);
    }

    ReleaseCapture();
}

void Window::onMouseMove(int pixelX, int pixelY, DWORD flags) {
    float dipX = DPIScale::pixelToDipsX(pixelX);
    float dipY = DPIScale::pixelToDipsY(pixelY);

    if ((flags & MK_LBUTTON) && _selection) {
        _selection->setBottomRight(D2D1::Point2F(dipX, dipY));
        InvalidateRect(_canvasHwnd, NULL, FALSE);
    }
}

void Window::onNotify(LPARAM lParam) {
    LPNMHDR lpnm = (LPNMHDR)lParam;

    switch (lpnm->code) {
        case TTN_GETDISPINFO:
        {
            LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
            lpttt->hinst = GetModuleHandle(NULL);
            UINT_PTR idButton = lpttt->hdr.idFrom;

            switch (idButton) {
                case ICON_NEW:
                    lpttt->lpszText = MAKEINTRESOURCE(IDS_TIPS_NEW);
                    break;

                case ICON_OPEN :
                    lpttt->lpszText = MAKEINTRESOURCE(IDS_TIPS_OPEN);
                    break;

                case ICON_SAVE:
                    lpttt->lpszText = MAKEINTRESOURCE(IDS_TIPS_SAVE);
                    break;

                case ICON_SAVEAS:
                    lpttt->lpszText = MAKEINTRESOURCE(IDS_TIPS_SAVEAS);
                    break;

                case ICON_UNDO:
                    lpttt->lpszText = MAKEINTRESOURCE(IDS_TIPS_UNDO);
                    break;

                case ICON_REDO:
                    lpttt->lpszText = MAKEINTRESOURCE(IDS_TIPS_REDO);
                    break;

                case ICON_PAINTBUCKET:
                    lpttt->lpszText = MAKEINTRESOURCE(IDS_TIPS_PAINTBUCKET);
                    break;

                case ICON_PAINTBRUSH:
                    lpttt->lpszText = MAKEINTRESOURCE(IDS_TIPS_PAINTBRUSH);
                    break;
            }
        }
        
        case TBN_DROPDOWN:
        {
            LPNMTOOLBAR lpnmTB = (LPNMTOOLBAR)lParam;

            switch (lpnmTB->iItem) {
                case ICON_WIDTH:
                {
                    RECT rc;

                    SendMessage(lpnmTB->hdr.hwndFrom, TB_GETRECT, (WPARAM)lpnmTB->iItem, (LPARAM)&rc);
                    MapWindowPoints(lpnmTB->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);

                    HMENU hMenuLoaded = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU2));
                    HMENU hPopupMenu = GetSubMenu(hMenuLoaded, 0);
                    TPMPARAMS tpm;

                    tpm.cbSize = sizeof(TPMPARAMS);
                    tpm.rcExclude = rc;

                    TrackPopupMenuEx(hPopupMenu,
                        TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
                        rc.left, rc.bottom, _mainHwnd, &tpm);

                    DestroyMenu(hMenuLoaded);
                }
            }
        }
    }
}

void Window::onCommand(WORD id) {
    switch (id) {
        case ICON_LINE:
            changeShapeType(MyShape::LINE);
            break;

        case ICON_RECTANGLE:
            changeShapeType(MyShape::RECTANGLE);
            break;

        case ICON_TRIANGLE:
            changeShapeType(MyShape::TRIANGLE);
            break;

        case ICON_ELLIPSE:
            changeShapeType(MyShape::ELLIPSE);
            break;

        case ICON_PAINTBRUSH:
            chooseColor(_brushColor);
            break;

        case ICON_PAINTBUCKET:
            chooseColor(_filledColor);
            break;

        case ID_SIZE_1:
            changeStrokeWidth(1.0f);
            break;

        case ID_SIZE_2:
            changeStrokeWidth(2.0f);
            break;

        case ID_SIZE_3:
            changeStrokeWidth(3.0f);
            break;

        case ID_SIZE_4:
            changeStrokeWidth(4.0f);
            break;

        case ID_SIZE_5:
            changeStrokeWidth(5.0f);
            break;

        case ID_FILE_NEW:
        case ICON_NEW:
        case ID_NEW:
            createNewObject();
            break;
            
        case ID_FILE_OPEN:
        case ICON_OPEN:
        case ID_OPEN:
            showOpenObjectDialog();
            break;

        case ID_FILE_SAVE:
        case ICON_SAVE:
        case ID_SAVE:
            showSaveObjectDialog();
            break;

        case ID_FILE_SAVEAS:
        case ICON_SAVEAS:
        case ID_SAVEAS:
            showSaveAsObjectDialog();
            break;

        case ICON_UNDO:
        case ID_UNDO:
            undoLastAction();
            break;

        case ICON_REDO:
        case ID_REDO:
            redoLastAction();
            break;

        case ID_FILE_EXIT:
            onClose();
            break;
    }
}

void Window::onPaint() {
    HRESULT hr = createGraphicResources();

    if (SUCCEEDED(hr)) {
        PAINTSTRUCT ps;
        BeginPaint(_canvasHwnd, &ps);

        _pRenderTarget->BeginDraw();
        _pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        _shapeContainer.draw(_pRenderTarget, _pBrush);

        if (_selection != NULL)
            _selection->draw(_pRenderTarget, _pBrush);

        _pRenderTarget->EndDraw();
        EndPaint(_canvasHwnd, &ps);
    }
}

void Window::onSetCursor(MouseMode mode) {
    _mouseMode = mode;

    LPWSTR cursor = NULL;
    switch (mode) {
    case ARROW:
        cursor = IDC_ARROW;
        break;

    case CROSS:
        cursor = IDC_CROSS;
        break;
    }

    _hCursor = LoadCursor(NULL, cursor);
    SetCursor(_hCursor);
}

void Window::changeShapeType(MyShape::ShapeType shapeType) {
    switch (_shapeType) {
    case MyShape::LINE:
        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_LINE, (LPARAM)LOWORD(TBSTATE_ENABLED));
        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_PAINTBUCKET, (LPARAM)TBSTATE_ENABLED);
        break;

    case MyShape::RECTANGLE:
        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_RECTANGLE, (LPARAM)LOWORD(TBSTATE_ENABLED));
        break;

    case MyShape::TRIANGLE:
        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_TRIANGLE, (LPARAM)LOWORD(TBSTATE_ENABLED));
        break;

    case MyShape::ELLIPSE:
        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_ELLIPSE, (LPARAM)LOWORD(TBSTATE_ENABLED));
        break;
    }

    switch (shapeType) {
    case MyShape::LINE:
        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_LINE, (LPARAM)LOWORD(TBSTATE_ENABLED | TBSTATE_CHECKED));
        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_PAINTBUCKET, (LPARAM)0);
        break;

    case MyShape::RECTANGLE:
        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_RECTANGLE, (LPARAM)LOWORD(TBSTATE_ENABLED | TBSTATE_CHECKED));
        break;

    case MyShape::TRIANGLE:
        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_TRIANGLE, (LPARAM)LOWORD(TBSTATE_ENABLED | TBSTATE_CHECKED));
        break;

    case MyShape::ELLIPSE:
        SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_ELLIPSE, (LPARAM)LOWORD(TBSTATE_ENABLED | TBSTATE_CHECKED));
        break;
    }

    _shapeType = shapeType;
}

void Window::changeStrokeWidth(float width) {
    _strokeWidth = width;
}

void Window::chooseColor(D2D1_COLOR_F& color) {
    CHOOSECOLOR cc;
    COLORREF custColors[16];

    ZeroMemory(&cc, sizeof(CHOOSECOLOR));
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = _canvasHwnd;
    cc.lpCustColors = (LPDWORD)custColors;
    cc.Flags =  CC_SOLIDCOLOR | CC_SHOWHELP;

    if (ChooseColor(&cc)) {
        float r = (float)GetRValue(cc.rgbResult) / 255;
        float g = (float)GetGValue(cc.rgbResult) / 255;
        float b = (float)GetBValue(cc.rgbResult) / 255;

        color = D2D1::ColorF(r, g, b);
    }
}

void Window::resetWindow() {
    _shapeContainer.clear();
    _redo.clear();
    _undo.clear();
    _shapeType = MyShape::LINE;
    _filledColor = D2D1::ColorF(D2D1::ColorF::White);
    _brushColor = D2D1::ColorF(D2D1::ColorF::Black);
    _strokeWidth = 1.0f;
    _filePath = L"Untitled.shapeobj";
    _isSaved = true;
    _isBinded = false;

    SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_REDO, (LPARAM)0);
    SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_UNDO, (LPARAM)0);
    changeShapeType(MyShape::LINE);
    InvalidateRect(_canvasHwnd, NULL, FALSE);
}

int Window::showSaveConfirmDialog() {
    wstring nl = (int)_filePath.size() > 50 ? L"\n" : L"";
    wstring text = L"Do you want to save changes to " + nl + _filePath;
    int msgboxID = MessageBoxW(
        _mainHwnd,
        text.c_str(),
        (LPCWSTR)L"Paint",
        MB_YESNOCANCEL | MB_DEFBUTTON1 | MB_TOPMOST
    );

    return msgboxID;
}

void Window::createNewObject() {
    if (!_isSaved) {
        int msgboxID = showSaveConfirmDialog();

        switch (msgboxID) {
        case IDYES:
            showSaveObjectDialog();
            break;

        case IDNO:
            // Do nothing
            break;

        case IDCANCEL:
            // Do nothing
            return;
        }
    }

    resetWindow();
}

HRESULT Window::showOpenObjectDialog() {
    CComPtr<IFileOpenDialog> pFileOpen = NULL;
    CComPtr<IShellItem> pShellResult = NULL;
    PWSTR pszFilePath = NULL;
    HRESULT hr = pFileOpen.CoCreateInstance(__uuidof(FileOpenDialog));

    if (SUCCEEDED(hr)) 
        hr = pFileOpen->SetOptions(FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST | 
            FOS_FILEMUSTEXIST);

    if (SUCCEEDED(hr)) 
        hr = pFileOpen->SetFileTypes(ARRAYSIZE(_fileTypes), _fileTypes);
    
    if (SUCCEEDED(hr)) 
        hr = pFileOpen->SetFileTypeIndex(1);

    if (SUCCEEDED(hr)) 
        hr = pFileOpen->SetDefaultExtension(L"paintobj");
    
    if (SUCCEEDED(hr)) 
        hr = pFileOpen->Show(NULL);

    if (SUCCEEDED(hr)) 
        hr = pFileOpen->GetResult(&pShellResult);

    if (SUCCEEDED(hr)) 
        hr = pShellResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    
    if (SUCCEEDED(hr)) {
        _filePath.assign(pszFilePath);
        _isBinded = true;
        _isSaved = true;
        _shapeContainer.loadFromFile(_filePath);
        InvalidateRect(_canvasHwnd, NULL, FALSE);
    }

    return hr;
}

HRESULT Window::showSaveObjectDialog() {
    if (_isSaved)
        return S_OK;

    if (_isBinded) {
        _shapeContainer.saveToFile(_filePath);
        _isSaved = true;
        return S_OK;
    }

    CComPtr<IFileSaveDialog> pFileSave = NULL;
    CComPtr<IShellItem> pShellResult = NULL;
    PWSTR pszFilePath = NULL;
    HRESULT hr = pFileSave.CoCreateInstance(__uuidof(FileSaveDialog));

    if (SUCCEEDED(hr))
        hr = pFileSave->SetOptions(FOS_OVERWRITEPROMPT | FOS_STRICTFILETYPES | 
            FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST | FOS_NOREADONLYRETURN);

    if (SUCCEEDED(hr))
        hr = pFileSave->SetFileTypes(ARRAYSIZE(_fileTypes), _fileTypes);

    if (SUCCEEDED(hr))
        hr = pFileSave->SetFileTypeIndex(1);

    if (SUCCEEDED(hr))
        hr = pFileSave->SetDefaultExtension(L"paintobj");

    if (SUCCEEDED(hr))
        hr = pFileSave->SetFileName(L"Untitled.paintobj");

    if (SUCCEEDED(hr))
        hr = pFileSave->SetTitle(L"Save");

    if (SUCCEEDED(hr))
        hr = pFileSave->Show(NULL);

    if (SUCCEEDED(hr))
        hr = pFileSave->GetResult(&pShellResult);

    if (SUCCEEDED(hr))
        hr = pShellResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

    if (SUCCEEDED(hr)) {
        _filePath.assign(pszFilePath);
        _shapeContainer.saveToFile(_filePath);
        _isBinded = true;
        _isSaved = true;
    }

    return hr;
}

HRESULT Window::showSaveAsObjectDialog() {
    CComPtr<IFileSaveDialog> pFileSave = NULL;
    CComPtr<IShellItem> pShellResult = NULL;
    PWSTR pszFilePath = NULL;
    HRESULT hr = pFileSave.CoCreateInstance(__uuidof(FileSaveDialog));

    if (SUCCEEDED(hr))
        hr = pFileSave->SetOptions(FOS_OVERWRITEPROMPT | FOS_STRICTFILETYPES |
            FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST | FOS_NOREADONLYRETURN);

    if (SUCCEEDED(hr))
        hr = pFileSave->SetFileTypes(ARRAYSIZE(_fileTypes), _fileTypes);

    if (SUCCEEDED(hr))
        hr = pFileSave->SetFileTypeIndex(1);

    if (SUCCEEDED(hr))
        hr = pFileSave->SetDefaultExtension(L"paintobj");

    if (SUCCEEDED(hr))
        hr = pFileSave->SetFileName(L"Untitled.paintobj");

    if (SUCCEEDED(hr))
        hr = pFileSave->SetTitle(L"Save as");

    if (SUCCEEDED(hr))
        hr = pFileSave->Show(NULL);

    if (SUCCEEDED(hr))
        hr = pFileSave->GetResult(&pShellResult);

    if (SUCCEEDED(hr))
        hr = pShellResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

    if (SUCCEEDED(hr)) {
        _filePath.assign(pszFilePath);
        _shapeContainer.saveToFile(_filePath);
        _isBinded = true;
        _isSaved = true;
    }

    return hr;
}

void Window::undoLastAction() {
    if (!_undo.empty()) {
        if (_redo.empty())
            SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_REDO, (LPARAM)TBSTATE_ENABLED);

        shared_ptr<MyShape::Shape> lastAction = _undo.back();
        _undo.pop_back();
        _shapeContainer.pop();
        _redo.push_back(lastAction);

        InvalidateRect(_canvasHwnd, NULL, FALSE);

        if (_undo.empty())
            SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_UNDO, (LPARAM)0);
    }
}

void Window::redoLastAction() {
    if (!_redo.empty()) {
        if (_undo.empty())
            SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_UNDO, (LPARAM)TBSTATE_ENABLED);

        shared_ptr<MyShape::Shape> lastAction = _redo.back();
        _redo.pop_back();
        _shapeContainer.push(lastAction);
        _undo.push_back(lastAction);

        InvalidateRect(_canvasHwnd, NULL, FALSE);

        if (_redo.empty())
            SendMessage(_toolbarHwnd, TB_SETSTATE, ICON_REDO, (LPARAM)0);
    }
}

HRESULT Window::insertShape(float x, float y) {
    try {
        _selection = MyShape::ShapeFactory::instance()->createShape(_shapeType);

        _selection->setTopLeft(D2D1::Point2F(x, y));
        _selection->setBottomRight(D2D1::Point2F(x, y));
        _selection->setBrushColor(_brushColor);
        _selection->setFilledColor(_filledColor);
        _selection->setStrokeWidth(_strokeWidth);
    }
    catch (bad_alloc) {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

HRESULT Window::createGraphicResources() {
    HRESULT hr = S_OK;

    if (_pRenderTarget == NULL) {
        RECT rc;
        GetClientRect(_canvasHwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = Direct2DFactory::instance()->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(_canvasHwnd, size),
            &_pRenderTarget);

        if (SUCCEEDED(hr)) {
            hr = _pRenderTarget->CreateSolidColorBrush(_brushColor, &_pBrush);
        }
    }

    return hr;
}

HWND Window::createToolbar() {
    const int imageListID = 0;
    const int numButtons = 13;
    const int bitmapSize = 32;
    const DWORD buttonStyles = BTNS_AUTOSIZE;

    HWND toolbarHwnd = CreateWindowEx(NULL, TOOLBARCLASSNAME, NULL,
        WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 0, 0, 0, 0,
        _mainHwnd, NULL, GetModuleHandle(NULL), NULL);

    if (toolbarHwnd == NULL)
        return NULL;

    _hImageList = ImageList_Create(bitmapSize, bitmapSize, ILC_COLOR24 | ILC_MASK, numButtons, 0);

    SendMessage(toolbarHwnd, TB_SETIMAGELIST, (WPARAM)imageListID, (LPARAM)_hImageList);

    TBADDBITMAP tbab[numButtons] = {
        { GetModuleHandle(NULL), ICON_NEW },
        { GetModuleHandle(NULL), ICON_OPEN },
        { GetModuleHandle(NULL), ICON_SAVE },
        { GetModuleHandle(NULL), ICON_SAVEAS },
        { GetModuleHandle(NULL), ICON_UNDO },
        { GetModuleHandle(NULL), ICON_REDO },
        { GetModuleHandle(NULL), ICON_WIDTH },
        { GetModuleHandle(NULL), ICON_LINE },
        { GetModuleHandle(NULL), ICON_RECTANGLE },
        { GetModuleHandle(NULL), ICON_TRIANGLE },
        { GetModuleHandle(NULL), ICON_ELLIPSE },
        { GetModuleHandle(NULL), ICON_PAINTBUCKET },
        { GetModuleHandle(NULL), ICON_PAINTBRUSH }
    };

    for (int i = 0; i < numButtons; i++)
        SendMessage(toolbarHwnd, TB_ADDBITMAP, (WPARAM)1, (LPARAM)&tbab[i]);

    TBBUTTON tbButtons[numButtons] = {
        { 0, ICON_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"New"},
        { 1, ICON_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Open"},
        { 2, ICON_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Save"},
        { 3, ICON_SAVEAS, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Save as"},
        { 4, ICON_UNDO, 0, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Undo"},
        { 5, ICON_REDO, 0, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Redo"},
        { 6, ICON_WIDTH, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN | BTNS_WHOLEDROPDOWN, {0}, 0, (INT_PTR)L"Width"},
        { 7, ICON_LINE, TBSTATE_ENABLED | TBSTATE_CHECKED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Line"},
        { 8, ICON_RECTANGLE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Rectangle"},
        { 9, ICON_TRIANGLE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Triangle"},
        { 10, ICON_ELLIPSE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Ellipse"},
        { 11, ICON_PAINTBUCKET, 0, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Color 1"},
        { 12, ICON_PAINTBRUSH, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Color 2"},
    };

    SendMessage(toolbarHwnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(toolbarHwnd, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);
    SendMessage(toolbarHwnd, TB_AUTOSIZE, 0, 0);
    SendMessage(toolbarHwnd, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_DRAWDDARROWS);
    ShowWindow(toolbarHwnd, TRUE);

    return toolbarHwnd;
}

HWND Window::createCanvas() {
    WNDCLASS wc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = canvasWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"CanvasChildWindow";

    RegisterClass(&wc);

    RECT rcMain, rcToolbar;
    GetClientRect(_mainHwnd, &rcMain);
    GetClientRect(_toolbarHwnd, &rcToolbar);

    HWND canvasHwnd = CreateWindowEx(
        NULL, L"CanvasChildWindow", NULL, WS_CHILD | WS_VISIBLE, 0, rcToolbar.bottom,
        rcMain.right, rcMain.bottom, _mainHwnd, NULL, GetModuleHandle(NULL), this
    );

    return canvasHwnd;
}

Window::Window() {
    _mainHwnd = NULL;
    _toolbarHwnd = NULL;
    _canvasHwnd = NULL;
    _mouseMode = ARROW;
    _shapeType = MyShape::LINE;
    _filledColor = D2D1::ColorF(D2D1::ColorF::White);
    _brushColor = D2D1::ColorF(D2D1::ColorF::Black);
    _strokeWidth = 1.0f;
    _selection = NULL;
    _filePath = L"Untitled.shapeobj";
    _isSaved = true;
    _isBinded = false;
    _fileTypes[0] = { L"Paint Object (*.paintobj)", L"*.paintobj" };
}

BOOL Window::create(PCWSTR lpWindownName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu) {
    WNDCLASS wc;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = mainWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);
    wc.lpszClassName = L"MainWindow";

    RegisterClass(&wc);

    _mainHwnd = CreateWindowEx(
        dwExStyle, L"MainWindow", lpWindownName, dwStyle, x, y,
        nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    return (_mainHwnd ? TRUE : FALSE);
}

HWND Window::window() {
    return _mainHwnd;
}
