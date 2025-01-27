#include "Constants.h"
class BoundingSphere
{
private:
	std::array<double, 3> centerPoint;
	double radius;

public:
	BoundingSphere(const std::array<double, 3>& center, double r);
	BoundingSphere(const BoundingSphere& other);
	~BoundingSphere();
	const std::array<double, 3>& GetCenterPoint() const;
	const double& GetRadius() const;
}; 
#pragma once
