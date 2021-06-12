#include "triangle.h"

CComPtr<ID2D1PathGeometry> MyShape::Triangle::getTriangle() {
    D2D1_POINT_2F pt1, pt2, pt3;
    CComPtr<ID2D1GeometrySink> pSink = NULL;
    CComPtr<ID2D1PathGeometry> pPathGeometry;
    HRESULT hr = S_OK;

    float width = _bottomRight.x - _topLeft.x;
    pt1 = D2D1::Point2F(_topLeft.x, max(_topLeft.y, _bottomRight.y));
    pt2 = D2D1::Point2F(_bottomRight.x, max(_topLeft.y, _bottomRight.y));
    pt3 = D2D1::Point2F(_topLeft.x + width / 2, min(_topLeft.y, _bottomRight.y));

    if (SUCCEEDED(hr)) {
        hr = Direct2DFactory::instance()->CreatePathGeometry(&pPathGeometry);

        if (SUCCEEDED(hr)) {
            hr = pPathGeometry->Open(&pSink);

            if (SUCCEEDED(hr)) {
                pSink->BeginFigure(pt1, D2D1_FIGURE_BEGIN_FILLED);
                pSink->AddLine(pt2);
                pSink->AddLine(pt3);
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

                hr = pSink->Close();
            }
        }
    }

    return pPathGeometry;
}

MyShape::Triangle::Triangle() = default;

void MyShape::Triangle::draw(ID2D1RenderTarget* pRenderTarget, CComPtr<ID2D1SolidColorBrush> pBrush) {
	CComPtr<ID2D1PathGeometry> triangle = getTriangle();
    pBrush->SetColor(_filledColor);
    pRenderTarget->FillGeometry(triangle, pBrush);
    pBrush->SetColor(_brushColor);
    pRenderTarget->DrawGeometry(triangle, pBrush, _strokeWidth);
}

MyShape::ShapeType MyShape::Triangle::getShapeType() {
    return TRIANGLE;
}

shared_ptr<MyShape::Shape> MyShape::Triangle::create() {
    return make_shared<MyShape::Triangle>();
}

shared_ptr<MyShape::Shape> MyShape::Triangle::parse(string buffer) {
    shared_ptr<Triangle> triangle = make_shared<Triangle>();
    vector<string> tokens = Tokenizer::spilit(buffer, "||");

    Point topLeft = Point::parse(tokens[0]);
    Point bottomRight = Point::parse(tokens[1]);
    Color filledColor = Color::parse(tokens[2]);
    Color brushColor = Color::parse(tokens[3]);
    FLOAT strokeWidth = stof(tokens[4]);

    triangle->setTopLeft(topLeft);
    triangle->setBottomRight(bottomRight);
    triangle->setFilledColor(filledColor);
    triangle->setBrushColor(brushColor);
    triangle->setStrokeWidth(strokeWidth);

    return triangle;
}
