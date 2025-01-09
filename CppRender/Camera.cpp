#include "Camera.h"

std::array<double, 3> Camera::GetPosition() const
{
	return position;
}

std::array<double, 3> Camera::GetDirection() const
{
	return direction;
}
