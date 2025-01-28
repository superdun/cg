#pragma once
#include "LightTypeEnum.h"
#include <array>
class Light
{
private:
	LightTypeEnum lightType;
	std::array<double, 3> position;
	std::array<double, 3> direction;
	double intensity;
public:
	Light(const LightTypeEnum& lightType, const std::array<double, 3>& position, const std::array<double, 3>& direction, const double& intensity);
	~Light();
	LightTypeEnum GetLightType() const;
	std::array<double, 3> GetPosition() const;
	std::array<double, 3> GetDirection() const;
	double GetIntensity() const;
};

