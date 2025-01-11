#pragma once
#include <array>
class Camera
{
private:
	std::array<double, 3> position;
	std::array<double, 3> direction;

public:
	Camera(const std::array<double, 3>& position, const std::array<double, 3>& direction);
	~Camera();
	std::array<double, 3> GetPosition() const;
	std::array<double, 3> GetDirection() const;
};

