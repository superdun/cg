#include "Camera.h"

Camera::Camera(const std::array<double, 3>& position, const std::array<double, 3>& direction)
	:position(position),direction(direction)
{
}

Camera::~Camera()
{
}

std::array<double, 3> Camera::GetPosition() const
{
	return position;
}

std::array<double, 3> Camera::GetDirection() const
{
	return direction;
}
