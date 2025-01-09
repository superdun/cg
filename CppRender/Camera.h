#pragma once
#include <array>
class Camera
{
private:
	std::array<double, 3> position;
	std::array<double, 3> direction;

public:
	Camera(std::array<double, 3> position, std::array<double, 3> direction);
	std::array<double, 3> GetPosition() const;
	std::array<double, 3> GetDirection() const;
};

