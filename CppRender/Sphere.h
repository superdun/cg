#pragma once
#include <array>
#include <stdexcept>
#include <cmath>
#include "VectorHelper.h"
#include "Constants.h"
class Sphere
{
private:
	std::array<double, 3> centerPoint;
	double radius;
	std::array<int, 3> color;
	double specular;
	double reflective;
public:
	Sphere(const std::array<double, 3>& center, double r,const std::array<int, 3> color, double specular, double reflective);
	~Sphere();
	std::array<double, 3> GetCenterPoint() const;
	std::array<int, 3> GetColor() const;
	double GetRadius() const;
	double GetSpecular() const;
	double GetReflective() const;
	double GetIntersectT(const std::array<double, 3>& rayOrigin, const std::array<double, 3>& directionVector) const;
	std::array<double, 3> GetNormalVector(const std::array<double, 3>& pointOnSurface) const;
};

