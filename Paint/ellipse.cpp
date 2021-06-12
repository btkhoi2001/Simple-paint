#include "ellipse.h"

D2D1_ELLIPSE MyShape::Ellipse::getEllipse() {
    const float radiusX = (_bottomRight.x - _topLeft.x) / 2;
    const float radiusY = (_bottomRight.y - _topLeft.y) / 2;
    const float x = _topLeft.x + radiusX;
    const float y = _topLeft.y + radiusY;

    return D2D1::Ellipse(D2D1::Point2F(x, y), abs(radiusX), abs(radiusY));
}

MyShape::Ellipse::Ellipse() = default;

void MyShape::Ellipse::draw(ID2D1RenderTarget* pRenderTarget, CComPtr<ID2D1SolidColorBrush> pBrush) {
    D2D1_ELLIPSE ellipse = getEllipse();
    pBrush->SetColor(_filledColor);
    pRenderTarget->FillEllipse(ellipse, pBrush);
    pBrush->SetColor(_brushColor);
    pRenderTarget->DrawEllipse(ellipse, pBrush, _strokeWidth);
}

MyShape::ShapeType MyShape::Ellipse::getShapeType() {
    return ELLIPSE;
}

shared_ptr<MyShape::Shape> MyShape::Ellipse::create() {
    return make_shared<MyShape::Ellipse>();
}

shared_ptr<MyShape::Shape> MyShape::Ellipse::parse(string buffer) {
    shared_ptr<Ellipse> ellipse = make_shared<Ellipse>();
    vector<string> tokens = Tokenizer::spilit(buffer, "||");

    Point topLeft = Point::parse(tokens[0]);
    Point bottomRight = Point::parse(tokens[1]);
    Color filledColor = Color::parse(tokens[2]);
    Color brushColor = Color::parse(tokens[3]);
    FLOAT strokeWidth = stof(tokens[4]);

    ellipse->setTopLeft(topLeft);
    ellipse->setBottomRight(bottomRight);
    ellipse->setFilledColor(filledColor);
    ellipse->setBrushColor(brushColor);
    ellipse->setStrokeWidth(strokeWidth);

    return ellipse;
}

