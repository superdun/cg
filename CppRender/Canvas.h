#pragma once
#include <array>
#include <vector>

#include "Constants.h"
#include "VectorHelper.h"
#include "Camera.h"
#include "Sphere.h"
#include "Light.h"
#include <wtypes.h>
#include "Model.h"
#include "Triangle.h"
class Canvas
{
private:

	const int canvasWidth;
	const int canvasHeight;
	signed int canvasMinX;
	signed int canvasMaxX;
	signed int canvasMinY;
	signed int canvasMaxY;
	const double viewportWidth;
	const double viewportHeight;
	const double viewportDistance;
	const double viewportCanvasWidthRate;
	const double viewportCanvasHeightRate;
	const double canvasViewportWidthRate;
	const double  canvasViewportHeightRate;
	const HDC& hMemDC;

	const Camera* camera;
	HBITMAP hBitmap;
	void* pixelData;
	
public:
	Canvas(const HDC& hMemDC, const int canvasWidth,
		const int canvasHeight, const Camera* camera,
		const double viewportWidth = 1,
		const double viewportHeight = 1,
		const double viewportDistance = 1);
	~Canvas();
	const Camera* getCamera() const;
	const int& getCanvasWidth() const;
	const int& getCanvasHeight() const;
	const int& getCanvasMinX() const;
	const int& getCanvasMaxX() const;
	const int& getCanvasMinY() const;
	const int& getCanvasMaxY() const;
	const double& getViewportWidth() const;
	const double& getViewportHeight() const;
	const double& getViewportDistance() const;
	const double& getViewportCanvasWidthRate() const;
	const double& getViewportCanvasHeightRate() const;
	const double& getCanvasViewportWidthRate() const;
	const double& getCanvasViewportHeightRate() const;
	double ConvertXToViewportCoordinate(const double& x) const;
	double ConvertYToViewportCoordinate(const double& x) const;
	int ConvertXToCanvasCoordinate(const double& x) const;
	int ConvertYToCanvasCoordinate(const double& y) const;
	std::pair<std::array<int, 2>, double> ConvertPointToCanvasCoordinateWithDepth(const std::array<double, 3>& positionWithDepth) const;
	void PutPixel(const int x, const int y, const COLORREF& color) const;
	void resetCanvas() const;
	HBITMAP& GetHBitmap();
};