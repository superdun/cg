#pragma once
#include <array>
#include <stdexcept>
#include <cmath>
#include "VectorHelper.h"
#include "BoundingSphere.h"


BoundingSphere::~BoundingSphere()
{
}



BoundingSphere::BoundingSphere(const std::array<double, 3>& center, double r)
	:centerPoint(center),radius(r)
{
}

BoundingSphere::BoundingSphere(const BoundingSphere& other)
	:centerPoint(other.centerPoint),radius(other.radius)
{
}

const std::array<double, 3>& BoundingSphere::GetCenterPoint() const
{
	return centerPoint;
}

const double& BoundingSphere::GetRadius() const
{
	return radius;
}


