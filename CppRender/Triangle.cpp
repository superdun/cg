#include "Triangle.h"

Triangle::Triangle(const std::array<double, 3>* v0Pointer, const std::array<double, 3>* v1Pointer, const std::array<double, 3>* v2Pointer, const std::array<int, 3> color)
    : v0Pointer(v0Pointer), v1Pointer(v1Pointer), v2Pointer(v2Pointer), color(color)
{
}

Triangle::~Triangle()
{
}

const std::array<double, 3>* Triangle::GetV0Pointer() const
{
    return v0Pointer;
}

const std::array<double, 3>* Triangle::GetV1Pointer() const
{
    return v1Pointer;
}

const std::array<double, 3>* Triangle::GetV2Pointer() const
{
    return v2Pointer;
}

const std::array<double, 3>& Triangle::GetV0() const
{
    return *v0Pointer;
}

const std::array<double, 3>& Triangle::GetV1() const
{
    return *v1Pointer;
}

const std::array<double, 3>& Triangle::GetV2() const
{
    return *v2Pointer;
}

const std::array<int, 3>& Triangle::GetColor() const
{
    return color;
}


