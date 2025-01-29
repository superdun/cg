#include "Pixel.h"

Pixel::Pixel(int x, int y, std::array<int, 3> color, double reciprocalDepth):
	position({ x,y }), color(color), reciprocalDepth(reciprocalDepth)
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

const double Pixel::GetReciprocalDepth() const
{
	return reciprocalDepth;
}


