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
    const std::array<double, 3>* GetV0Pointer() const;
    const std::array<double, 3>* GetV1Pointer() const;
    const std::array<double, 3>* GetV2Pointer() const;
    const std::array<double, 3>& GetV0() const;
    const std::array<double, 3>& GetV1() const;
    const std::array<double, 3>& GetV2() const;
    const std::array<int, 3>& GetColor() const;
    ~Triangle();
};
