#pragma once
#include <array>
class Triangle
{
private:
    const std::array<double, 3> v0;
    const std::array<double, 3> v1;
    const std::array<double, 3> v2;
    const double h0;
    const double h1;
    const double h2;
    const std::array<int, 3> color;
    const std::array<double, 3> normal;
public:
    Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2, const std::array<int, 3> color);
    Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2, const std::array<int, 3> color, const double h0, const double h1, const double h2);
    Triangle(const Triangle& other);
    const std::array<double, 3>& GetV0() const;
    const std::array<double, 3>& GetV1() const;
    const std::array<double, 3>& GetV2() const;
    const std::array<int, 3>& GetColor() const;
    const double GetH0() const;
    const double GetH1() const;
    const double GetH2() const;
    const std::array<double, 3>& GetNormal() const;
    ~Triangle();
};
