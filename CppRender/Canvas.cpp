#include "Canvas.h"



Canvas::Canvas(const int& canvasWidth, const int& canvasHeight, const Camera& camera)
	:canvasWidth(canvasWidth), canvasHeight(canvasHeight), camera(camera),
	canvasMinX(canvasWidth / -2), canvasMaxX(canvasWidth / 2),
	canvasMinY(canvasHeight / -2), canvasMaxY(canvasHeight / 2)
{
}

Canvas::~Canvas()
{
}

void Canvas::RunRender()
{
	for (signed int w = canvasMinX; w < canvasMaxX; w++)
	{
		for (signed int h = canvasMinY; h < canvasMaxY; h++) {

		}
	}
}
