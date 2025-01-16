#pragma once
#include <array>
#include <vector>
#include "Pixel.h"
class RasterizationRender
{
public:
	RasterizationRender();
	~RasterizationRender();
	std::vector<double> Interpolate(int i0, double d0, int i1, double d1) const;
	std::vector<Pixel*> DrawLine(const std::array<int,2> p0,const std::array<int,2> p1,const std::array<int,3> color) const;
	std::vector<Pixel*> DrawWireFrameTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const std::array<int, 3> color) const;
	std::vector<Pixel*> DrawFilledTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const std::array<int, 3> color) const;

};

