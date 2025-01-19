#include "Canvas.h"
#include <iostream>
#include <thread>


void Canvas::resetCanvas() const
{
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = rect.left + canvasWidth;
    rect.bottom = rect.top + canvasHeight;

    COLORREF colorRef = RGB(Constants::BACKGROUND_COLOR[0], Constants::BACKGROUND_COLOR[1], Constants::BACKGROUND_COLOR[2]);
    HBRUSH brush = CreateSolidBrush(colorRef); // Create a brush with the specified color
    FillRect(hMemDC, &rect, brush);
    DeleteObject(brush);
}


void Canvas::PutPixel(const int x,const int y ,const COLORREF& color) const
{
    SetPixel(hMemDC, x, y, color);
}




Canvas::Canvas(const HDC& hMemDC, const int canvasWidth, const int canvasHeight, const Camera* camera, const double viewportWidth, const double viewportHeight, const double viewportDistance)
    : hMemDC(hMemDC), canvasWidth(canvasWidth), canvasHeight(canvasHeight), camera(camera),
    canvasMinX(canvasWidth / -2), canvasMaxX(canvasWidth / 2),
    canvasMinY(canvasHeight / -2), canvasMaxY(canvasHeight / 2), viewportWidth(viewportWidth), viewportHeight(viewportHeight),
    viewportDistance(viewportDistance), viewportCanvasWidthRate(viewportWidth / canvasWidth), viewportCanvasHeightRate(viewportHeight / canvasHeight),
    canvasViewportWidthRate(canvasWidth / viewportWidth), canvasViewportHeightRate(canvasHeight / viewportHeight)

{
}

Canvas::~Canvas()
{
}


double Canvas::ConvertXToViewportCoordinate(const double& x) const
{
    return (x - canvasMaxX) * viewportCanvasWidthRate;
}

double Canvas::ConvertYToViewportCoordinate(const double& y) const
{
    return (-y + canvasMaxY) * viewportCanvasHeightRate;
}

int Canvas::ConvertXToCanvasCoordinate(const double& x) const
{
    return x * canvasViewportWidthRate + canvasMaxX;
}

int Canvas::ConvertYToCanvasCoordinate(const double& y) const
{
    return canvasMaxY - y * canvasViewportHeightRate;
}

std::array<int, 2> Canvas::ConvertPointToCanvasCoordinate(const std::array<double, 2>& position) const
{
    std::array<int, 2> screenPosition;
    screenPosition[0] = ConvertXToCanvasCoordinate(position[0]);
    screenPosition[1] = ConvertYToCanvasCoordinate(position[1]);
    return screenPosition;
}

//const std::vector<Object*>& Canvas::getObjects() const
//{
//    return objects;
//}
//const std::vector<Light*>& Canvas::getLights() const
//{
//    return lights;
//}
const Camera* Canvas::getCamera() const
{
    return camera;
}
const int& Canvas::getCanvasWidth() const
{
    return canvasWidth;
}

const int& Canvas::getCanvasHeight() const
{
    return canvasHeight;
}
const int& Canvas::getCanvasMinX() const
{
    return canvasMinX;
}
const int& Canvas::getCanvasMaxX() const
{
    return canvasMaxX;
}
const int& Canvas::getCanvasMinY() const
{
    return canvasMinY;
}
const int& Canvas::getCanvasMaxY() const
{
    return canvasMaxY;
}
const double& Canvas::getViewportWidth() const
{
    return viewportWidth;
}
const double& Canvas::getViewportHeight() const
{
    return viewportHeight;
}
const double& Canvas::getViewportDistance() const
{
    return viewportDistance;
}
const double& Canvas::getViewportCanvasWidthRate() const
{
    return viewportCanvasWidthRate;
}
const double& Canvas::getViewportCanvasHeightRate() const
{
    return viewportCanvasHeightRate;
}
const double& Canvas::getCanvasViewportWidthRate() const
{
    return canvasViewportWidthRate;
}
const double& Canvas::getCanvasViewportHeightRate() const
{
    return canvasViewportHeightRate;
}