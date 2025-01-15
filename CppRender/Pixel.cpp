#include "Pixel.h"

Pixel::Pixel(int x, int y, std::array<int, 3> color):
	position({ x,y }), color(color)
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

