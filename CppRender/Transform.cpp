#include "Transform.h"

Transform::Transform(const double scale, const std::array<double, 3>& rotate, const std::array<double, 3>& translation)
	:scale(scale), rotate(rotate), translation(translation)
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
