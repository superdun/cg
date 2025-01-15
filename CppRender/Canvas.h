#pragma once
#include <array>
#include <vector>

#include "Constants.h"
#include "VectorHelper.h"
#include "Camera.h"
#include "Sphere.h"
#include "Light.h"
#include <wtypes.h>
#include "RayTraceRender.h"
#include "RasterizationRender.h"
class Canvas
{
private:
	
	const int canvasWidth;
	const int canvasHeight;
	signed int canvasMinX;
	signed int canvasMaxX;
	signed int canvasMinY;
	signed int canvasMaxY;
	const double viewportWidth ;
	const double viewportHeight;
	const double viewportDistance;
	const double viewportCanvasWidthRate;
	const double viewportCanvasHeightRate;
	const HDC& hMemDC;
	const RayTraceRender* renderRayTrace;
	const RasterizationRender* renderRasterizationTrace;
	const Camera* camera;
	void resetCanvas() const;
	void RenderSectionByRayTrace(int startY, int endY, std::vector<std::vector<COLORREF>>& buffer) const;
	void MergeBuffer(std::vector<std::vector<COLORREF>>& buffer) const;
	 
public:
	Canvas(const RayTraceRender* renderRayTrace, const RasterizationRender* renderRasterizationTrace,const HDC& hMemDC, const int canvasWidth,
		const int canvasHeight,const Camera* camera,
		const double viewportWidth = 1,
		const double viewportHeight = 1,
		const double viewportDistance = 1);
	~Canvas();
	void RunRenderByRayTrace()  const;
	void RunRenderByRasterization() const;
	//void PutPixel();
};

