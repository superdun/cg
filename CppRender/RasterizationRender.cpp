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
 
std::vector<Pixel*> RasterizationRender::DrawLine(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 3> color) const
{
	std::array<int, 2> p0_new = p0;
	std::array<int, 2> p1_new = p1;
	std::vector<Pixel*> result;
	const bool steep = abs(p0_new[1] - p1_new[1]) > abs(p0_new[0] - p1_new[0]);
	if (steep)
	{
		
		if (p0_new[1]> p1_new[1])
		{
			std::swap(p0_new, p1_new);
		}
		auto xList = Interpolate(p0_new[1], p0_new[0], p1_new[1], p1_new[0]);
		for (int p = p0_new[1]; p <= p1_new[1]; p++)
		{
			result.push_back(new Pixel(xList[p - p0_new[1]], p, color));
		}
		
	}
	else {
		if (p0_new[0] > p1_new[0])
		{
			std::swap(p0_new, p1_new);
		}
		auto yList = Interpolate(p0_new[0], p0_new[1], p1_new[0], p1_new[1]);
		for (int p = p0_new[0]; p <= p1_new[0]; p++)
		{
			result.push_back(new Pixel(p, yList[p - p0_new[0]], color));
		}
	}

	return result;
}

std::vector<Pixel*> RasterizationRender::DrawWireFrameTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const std::array<int, 3> color) const
{


	std::vector<Pixel*> result;
	auto line1 = DrawLine(p0, p1, color);
	auto line2 = DrawLine(p1, p2, color);
	auto line3 = DrawLine(p0, p2, color);
	// Insert the resulting pixels from each line into the result vector
	result.insert(result.end(), line1.begin(), line1.end());
	result.insert(result.end(), line2.begin(), line2.end());
	result.insert(result.end(), line3.begin(), line3.end());
	return result;
}

std::vector<Pixel*> RasterizationRender::DrawFilledTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const std::array<int, 3> color) const
{
	std::vector<Pixel*> result;
	std::array<int, 2> p0_new = p0;
	std::array<int, 2> p1_new = p1;
	std::array<int, 2> p2_new = p2;
	if (p0_new[1]>p1_new[1])
	{
		std::swap(p0_new, p1_new);
	}
	if (p0_new[1] > p2_new[1])
	{
		std::swap(p0_new, p2_new);
	}
	if (p1_new[1] > p2_new[1])
	{
		std::swap(p1_new, p2_new);
	}
	auto xListLow = Interpolate(p0_new[1], p0_new[0], p2_new[1], p2_new[0]);
	auto xListHigher = Interpolate(p1_new[1], p1_new[0], p2_new[1], p2_new[0]);
	auto xListHighest = Interpolate(p0_new[1], p0_new[0], p1_new[1], p1_new[0]);
	
	
	for (int y = p0_new[1]; y <= p2_new[1]; y++)
	{
		auto xLow = xListLow[y - p0_new[1]];
		std::array<int, 2> pOnEdge1 = { xLow,y };
		std::array<int, 2> pOnEdge2;
		if (y< p1_new[1])
		{	
			
			pOnEdge2 = { (int)xListHighest[y- p0_new[1]],y};

		}
		else {
			pOnEdge2 = { (int)xListHigher[y - p1_new[1]],y };
		}
		if (pOnEdge1[0]> pOnEdge2[0])
		{
			std::swap(pOnEdge1, pOnEdge2);
		}
		for (int x = pOnEdge1[0]; x <= pOnEdge2[0]; x++)
		{
			result.push_back(new Pixel(x,y,color));
		}

	}


	return result;
}


