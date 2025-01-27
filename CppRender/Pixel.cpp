#include "Pixel.h"

Pixel::Pixel(int x, int y, std::array<int, 3> color, double depth):
	position({ x,y }), color(color), depth(depth)
{
}

Pixel::~Pixel()
{
}

const std::array<int, 2>& Pixel::GetPosition() const
{
	return position;
}

const std::array<int, 3>& Pixel::GetColor() const
{
	return color;
}

const double Pixel::GetDepth() const
{
	return depth;
}


