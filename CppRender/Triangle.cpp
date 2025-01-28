#include "Triangle.h"
#include "VectorHelper.h"

Triangle::Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2, const std::array<int, 3> color)
    : v0(v0), v1(v1), v2(v2), color(color), h0(1), h1(1), h2(1), normal(VectorHelper::GetNormal(v0, v1, v2))
{
}
Triangle::Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2, const std::array<int, 3> color, const double h0, const double h1, const double h2)
    : v0(v0), v1(v1), v2(v2), color(color), h0(h0), h1(h1), h2(h2), normal(VectorHelper::GetNormal(v0, v1, v2))
{
}

Triangle::Triangle(const Triangle& other)
    : v0(other.v0), v1(other.v1), v2(other.v2), color(other.color), h0(other.h0), h1(other.h1), h2(other.h2),normal(other.normal)
{
}

Triangle::~Triangle()
{
}

const std::array<double, 3>& Triangle::GetV0() const
{
    return v0;
}

const std::array<double, 3>& Triangle::GetV1() const
{
    return v1;
}

const std::array<double, 3>& Triangle::GetV2() const
{
    return v2;
}

const std::array<int, 3>& Triangle::GetColor() const
{
    return color;
}

const double Triangle::GetH0() const
{
    return h0;
}

const double Triangle::GetH1() const
{
    return h1;
}

const double Triangle::GetH2() const
{
    return h2;
}

const std::array<double, 3>& Triangle::GetNormal() const
{
    return normal;
}

void Triangle::SetH0(const double h0)
{
    this->h0 = h0;
}

void Triangle::SetH1(const double h1)
{
    this->h1 = h1;
}

void Triangle::SetH2(const double h2)
{
    this->h2 = h2;
}

