#pragma once
#include <array>
#include <vector>

#include "Constants.h"
#include "VectorHelper.h"
#include "Camera.h"
#include "Sphere.h"
#include "Light.h"
class Canvas
{
private:
	
	int canvasWidth;
	int canvasHeight;
	signed int canvasMinX;
	signed int canvasMaxX;
	signed int canvasMinY;
	signed int canvasMaxY;


	Camera camera;

public:
	Canvas(const int& canvasWidth, const int& canvasHeight,const Camera& camera);
	~Canvas();
	void RunRender();
	//void PutPixel();
};

