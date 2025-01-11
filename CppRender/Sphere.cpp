#include "Sphere.h"




Sphere::Sphere(const std::array<double, 3>& center, double r, const std::array<int, 3> color, double specular, double reflective)
	: centerPoint(center), radius(r), color(color), specular(specular), reflective(reflective)
{
	if (radius < 0) {
		throw std::invalid_argument("Radius cannot be negative.");
	}
}

Sphere::~Sphere()
{
}

std::array<double, 3> Sphere::GetCenterPoint() const
{
	return centerPoint;
}

std::array<int, 3> Sphere::GetColor() const
{
	return color;
}

double Sphere::GetRadius() const
{
	return radius;
}

double Sphere::GetSpecular() const
{
	return specular;
}

double Sphere::GetReflective() const
{
	return reflective;
}

double Sphere::GetIntersectT(const std::array<double, 3>& pointOrigin, const std::array<double, 3>& directionVector) const
{
	const std::array<double, 3> rayVector =VectorHelper::VectorAdd(pointOrigin, directionVector);
	const std::array<double, 3> coVector =VectorHelper::VectorSub(pointOrigin, centerPoint );
	const double a =VectorHelper::VectorDot(rayVector, rayVector);
	const double b = 2 *VectorHelper::VectorDot(rayVector, coVector);
	const double c =VectorHelper::VectorDot(coVector, coVector) - radius * radius;
	const double discriminant = b * b - 4 * a * c;
	if (discriminant<0)
	{
		return Constants::Infinity;
	}
	const double t1 = (-b + sqrt(discriminant)) / (2 * a);
	const double t2 = (-b - sqrt(discriminant)) / (2 * a);
	if (t1>t2 && t2>Constants::Infinitesimal && t2<Constants::Infinity)
	{
		return t2;
	}
	if (t1 <= t2 && t1 > Constants::Infinitesimal && t1 < Constants::Infinity)
	{
		return t1;
	}
	return Constants::Infinity;
}

std::array<double, 3> Sphere::GetNormalVector(const std::array<double, 3>& pointOnSurface) const
{
	const std::array<double, 3> normalDeirecttion =VectorHelper::VectorSub(pointOnSurface, centerPoint);
	return VectorHelper::VectorNormalize(normalDeirecttion);
}
