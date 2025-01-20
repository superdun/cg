#ifndef VECTORHELPER_H
#define VECTORHELPER_H

#include <array>
#include <cmath> // 为了使用 sqrt 和 acos

class VectorHelper
{
public:
    static std::array<double, 3> VectorAdd(const std::array<double, 3>& point, const std::array<double, 3>& v);
    static std::array<int, 3> ColorVectorAdd(const std::array<int, 3>& v1, const std::array<int, 3>& v2);
    static std::array<double, 3> VectorSub(const std::array<double, 3>& v1, const std::array<double, 3>& v2);
    static std::array<double, 3> VectorScale(const std::array<double, 3>& v, double scale);
    static std::array<int, 3> ColorVectorScale(const std::array<int, 3>& v, double scale);
    static double VectorDot(const std::array<double, 3>& v1, const std::array<double, 3>& v2);
    static std::array<double, 3> VectorProduct(const std::array<double, 3>& v1, const std::array<double, 3>& v2);
    static double VectorLength(const std::array<double, 3>& v);
    static std::array<double, 3> VectorNormalize(const std::array<double, 3>& v);
    static double AngleOfVectors(const std::array<double, 3>& v1, const std::array<double, 3>& v2);
    static std::array<double, 3> GetReflectVector(const std::array<double, 3>& v1, const std::array<double, 3>& normal);
    static  std::array<double, 3> VecRotate(const std::array<double, 3>& vector, double angleWithAxisX, double angleWithAxisY, double angleWithAxisZ);
    static std::array<std::array<double, 4>, 4> BuildModelCompositeMatrix(
        const std::array<double, 3>& scale,
        const std::array<double, 3>& angleWithAxis,
        const std::array<double, 3>& translate,
        const double d);
    static std::array<std::array<double, 4>, 4> BuildCameraCompositeMatrix(
        const std::array<double, 3>& angleWithAxis,
        const std::array<double, 3>& translate,
        const double d);
};

#endif // VECTORHELPER_H
