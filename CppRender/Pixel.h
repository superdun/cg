#pragma once
#include <array>
class Pixel
{
private:
	const std::array<int, 2> position;
	const std::array<int, 3> color;
	const double reciprocalDepth;
public:
	Pixel(int x, int y, std::array<int,3> color, double reciprocalDepth);
	~Pixel();
	const std::array<int, 2>& GetPosition() const;
	const std::array<int, 3>& GetColor() const;
	const double GetReciprocalDepth() const;
};

