#pragma once
#include <array>
class Triangle
{
private:
    const std::array<double, 3>* v0Pointer;
    const std::array<double, 3>* v1Pointer;
    const std::array<double, 3>* v2Pointer;
    const std::array<int, 3> color;

public:
    Triangle(const std::array<double, 3>* v0Pointer, const std::array<double, 3>* v1Pointer, const std::array<double, 3>* v2Pointer, const std::array<int, 3> color);
    const std::array<double, 3>* getV0Pointer() const;
    const std::array<double, 3>* getV1Pointer() const;
    const std::array<double, 3>* getV2Pointer() const;
    const std::array<int, 3>& getColor() const;
    ~Triangle();
};
