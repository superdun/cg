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

HBITMAP& Canvas::GetHBitmap()
{
	return hBitmap;
}


void Canvas::PutPixel(const int x,const int y ,const COLORREF& color) const
{
    if (x < 0 || x >= canvasWidth || y < 0 || y >= canvasHeight) return;

    // Calculate the pixel index (assuming 32 bits per pixel)
    int index = (y * canvasWidth + x) * 4; // 4 bytes per pixel for 32-bit

    // Set the pixel color (BGRA format for 32-bit)
    ((BYTE*)pixelData)[index + 0] = GetBValue(color); // Blue
    ((BYTE*)pixelData)[index + 1] = GetGValue(color); // Green
    ((BYTE*)pixelData)[index + 2] = GetRValue(color); // Red
    ((BYTE*)pixelData)[index + 3] = 255;              // Alpha (opaque)
}




Canvas::Canvas(const HDC& hMemDC, const int canvasWidth, const int canvasHeight, const Camera* camera, const double viewportWidth, const double viewportHeight, const double viewportDistance)
    : hMemDC(hMemDC), canvasWidth(canvasWidth), canvasHeight(canvasHeight), camera(camera),
    canvasMinX(canvasWidth / -2), canvasMaxX(canvasWidth / 2),
    canvasMinY(canvasHeight / -2), canvasMaxY(canvasHeight / 2), viewportWidth(viewportWidth), viewportHeight(viewportHeight),
    viewportDistance(viewportDistance), viewportCanvasWidthRate(viewportWidth / canvasWidth), viewportCanvasHeightRate(viewportHeight / canvasHeight),
    canvasViewportWidthRate(canvasWidth / viewportWidth), canvasViewportHeightRate(canvasHeight / viewportHeight)

{
    // Define the BITMAPINFO structure
    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo, sizeof(BITMAPINFO));
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = canvasWidth;
    bitmapInfo.bmiHeader.biHeight = -canvasHeight;  // Negative height for top-down bitmap
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;      // 32 bits per pixel (ARGB)
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    // Create a DIB section
    hBitmap = CreateDIBSection(hMemDC, &bitmapInfo, DIB_RGB_COLORS, &pixelData, NULL, 0);

    // Check for errors
    if (!hBitmap) {
        // Handle errors here
        throw std::runtime_error("Failed to create DIB section.");
    }

    // Optionally fill the pixelData with a default color (e.g., black)
    memset(pixelData, 0xFF, canvasWidth * canvasHeight * 4); // Clear to black (for 32-bit)


}

Canvas::~Canvas()
{
    // Clean up
	if (hBitmap) {
		DeleteObject(hBitmap);
	}
	if (pixelData) {
		// No need to delete pixelData; it's managed by the DIB section
	}
	//delete camera;
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

std::pair<std::array<int, 2>, double> Canvas::ConvertPointToCanvasCoordinateWithDepth(const std::array<double, 3>& positionWithDepth) const
{
    std::array<int, 2> screenPosition;
    screenPosition[0] = positionWithDepth[0]+ canvasMaxX;
    screenPosition[1] = canvasMaxY - positionWithDepth[1];
    return std::make_pair(screenPosition, positionWithDepth[2]);
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