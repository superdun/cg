
#include "Plane.h"
#include "VectorHelper.h"

Plane::Plane(const std::array<double, 3>& normal, const double D)
    :normal(normal),D(D)
{

}


const std::array<double, 3>& Plane::GetNormal() const
{
    return normal;
}

const double Plane::GetD() const
{
    return D;
}

