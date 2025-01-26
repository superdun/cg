#pragma once
#include <array>
class Plane
{

    // NP+D=0
private:
    const double D;
    const std::array<double, 3> normal;
public:
    Plane(const std::array<double, 3>& normal,const double D);
    const std::array<double, 3>& GetNormal() const;
    const double GetD() const;
};

