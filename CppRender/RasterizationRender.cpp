#include "RasterizationRender.h"
#include <algorithm>
RasterizationRender::RasterizationRender()
{
}

RasterizationRender::~RasterizationRender()
{
}

std::vector<double>  RasterizationRender::Interpolate(int i0, double d0, int i1, double d1) const
{
	std::vector<double> result;
	if (i0 == i1)
	{
		result.push_back(d0);
		return result;
	}
	const double a = (d1 - d0) / (i1 - i0);
	double d = d0;
	for (int i = i0; i <= i1; i++)
	{
		result.push_back(d);
		d += a;
	}
	return result;
}
 
std::vector<Pixel*> RasterizationRender::DrawLine(const std::array<int, 2> p0_in, const std::array<int, 2> p1_in, const std::array<int, 3> color) const
{
	std::array<int, 2> p0 = p0_in;
	std::array<int, 2> p1 = p1_in;
	std::vector<Pixel*> result;
	const bool steep = abs(p0[1] - p1[1]) > abs(p0[0] - p1[0]);
	if (steep)
	{
		
		if (p0[1]> p1[1])
		{
			std::swap(p0, p1);
		}
		auto xList = Interpolate(p0[1], p0[0], p1[1], p1[0]);
		for (int p = p0[1]; p <= p1[1]; p++)
		{
			result.push_back(new Pixel(xList[p - p0[1]], p, color));
		}
		
	}
	else {
		if (p0[0] > p1[0])
		{
			std::swap(p0, p1);
		}
		auto yList = Interpolate(p0[0], p0[1], p1[0], p1[1]);
		for (int p = p0[0]; p <= p1[0]; p++)
		{
			result.push_back(new Pixel(p, yList[p - p0[0]], color));
		}
	}

	return result;
}
