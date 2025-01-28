#include "Light.h"

Light::Light(const LightTypeEnum& lightType, const std::array<double, 3>& position, const std::array<double, 3>& direction, const double& intensity)
	:lightType(lightType), position(position), direction(direction), intensity(intensity)
{
}

Light::~Light()
{
}

LightTypeEnum Light::GetLightType() const
{
	return lightType;
}

std::array<double, 3> Light::GetPosition() const
{
	return position;
}

std::array<double, 3> Light::GetDirection() const
{
	return direction;
}

double Light::GetIntensity() const
{
	return intensity;
}