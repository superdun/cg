#pragma once
#include <array>
#include <vector>
#include "Pixel.h"
#include "Camera.h"
#include "Model.h"
#include "Canvas.h"
#include "ModelInstance.h"

class RasterizationRender
{
private:
	const Camera* camera;
	Canvas* canvas; 
	const std::vector<ModelInstance*>& instances;
public:
	RasterizationRender(Canvas* canvas, const std::vector<ModelInstance*>& instances, const Camera* camera);
	~RasterizationRender();
	std::vector<double> Interpolate(int i0, double d0, int i1, double d1) const;
	std::vector<Pixel*> DrawLine(const std::array<int,2> p0,const std::array<int,2> p1,const std::array<int,3> color) const;
	std::vector<Pixel*> DrawWireFrameTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const std::array<int, 3>& color) const;
	std::vector<Pixel*> DrawFilledTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const std::array<int, 3>& color) const;
	std::vector<Pixel*> DrawShadedTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2,const double h1, const double h2,const double h3, const std::array<int, 3>& color) const;
	std::array<double, 2> ProjectVertex(const std::array<double, 3>& vertex, const double d, const std::array<double, 3>& modelPosition) const;
	std::vector<Pixel*> RenderInstance(const ModelInstance& model, const std::array< std::array<double, 4>, 4>& matrix_camera) const;
	void RunRender() const;
	void SetCanvas( Canvas* canvas);
};

