#pragma once
#include <array>
class Constants
{
public:
	static constexpr std::array<int, 3> BACKGROUND_COLOR = { 255, 255, 255 };
	static constexpr double PI = 3.14159265358979323846;
	static constexpr double Infinity = 1e8;
	static constexpr double Infinitesimal = 1e-6;
	static constexpr int Max_Depth = 2;
};

