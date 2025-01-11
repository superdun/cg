#include "Light.h"

Light::Light(const LightTypeEnum& lightType, const std::array<double, 3>& position, const std::array<double, 3>& direction, const double& intensity)
	:lightType(lightType), position(position), direction(direction), intensity(intensity)
{
}

Light::~Light()
{
}

LightTypeEnum Light::getLightType() const
{
	return lightType;
}

std::array<double, 3> Light::getPosition() const
{
	return position;
}

std::array<double, 3> Light::getDirection() const
{
	return direction;
}

double Light::getIntensity() const
{
	return intensity;
}