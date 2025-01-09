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
};

