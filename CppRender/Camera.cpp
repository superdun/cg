#include "Camera.h"

Camera::Camera( std::array<double, 3> position,  std::array<double, 3> direction)
	:position(position), direction(direction)
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

double Camera::GetAngleY() const
{
	return angelY;
}

void Camera::Move() 
{
	angelY += 0.01;
	position[1] = position[1] + 0.01;
	position[2] = position[2]+0.01;
}

void Camera::Forward()
{
	position[2] = position[2] + 0.01;
}

void Camera::TurnAround() 
{
}
