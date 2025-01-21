#include "Transform.h"

Transform::Transform()
	:scale(1), rotate({0,0,0}), translation({0,0,0}), angle(0)
{
}

Transform::Transform(const double scale, const double angle, const std::array<double, 3>& rotate, const std::array<double, 3>& translation)
	:scale(scale), rotate(rotate), translation(translation), angle(angle)
{
}

Transform::~Transform()
{
}

const double& Transform::GetScale() const
{
    return scale;
}

const std::array<double, 3>& Transform::GetRotate() const
{
	return rotate;
}

const std::array<double, 3>& Transform::GetTranslation() const
{
	return translation;
}

const double& Transform::GetAngle() const
{
	return angle;
}

const void Transform::SetRotateState(const std::array<double, 3>& rotate, const double angle)
{
	this->rotate = rotate;
	this->angle = angle;

}
