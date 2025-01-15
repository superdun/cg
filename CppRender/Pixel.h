#pragma once
#include <array>
class Pixel
{
private:
	const std::array<int, 2> position;
	const std::array<int, 3> color;
public:
	Pixel(int x, int y, std::array<int,3> color);
	~Pixel();
	const std::array<int, 2>& GetPosition() const;
	const std::array<int, 3>& GetColor() const;
};

