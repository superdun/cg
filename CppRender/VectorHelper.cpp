#include "VectorHelper.h"
#include "Constants.h"

std::array<double, 3> VectorHelper::VectorAdd(const std::array<double, 3>& point, const std::array<double, 3>& v) {
    std::array<double, 3> result;
    result[0] = point[0] + v[0];
    result[1] = point[1] + v[1];
    result[2] = point[2] + v[2];
    return result;
}

std::array<double, 3> VectorHelper::VectorSub(const std::array<double, 3>& v1, const std::array<double, 3>& v2) {
    std::array<double, 3> result;
    result[0] = v1[0] - v2[0];
    result[1] = v1[1] - v2[1];
    result[2] = v1[2] - v2[2];
    return result;
}

std::array<double, 3> VectorHelper::VectorScale(const std::array<double, 3>& v, double scale) {
    std::array<double, 3> result;
    result[0] = v[0] * scale;
    result[1] = v[1] * scale;
    result[2] = v[2] * scale;
    return result;
}

double VectorHelper::VectorDot(const std::array<double, 3>& v1, const std::array<double, 3>& v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

std::array<double, 3> VectorHelper::VectorProduct(const std::array<double, 3>& v1, const std::array<double, 3>& v2) {
    std::array<double, 3> result;
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return result;
}

double VectorHelper::VectorLength(const std::array<double, 3>& v) {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

std::array<double, 3> VectorHelper::VectorNormalize(const std::array<double, 3>& v) {
    double length = VectorLength(v);
    return VectorScale(v, 1 / length);
}

double VectorHelper::AngleOfVectors(const std::array<double, 3>& v1, const std::array<double, 3>& v2) {
    return acos(VectorDot(v1, v2) / (VectorLength(v1) * VectorLength(v2))) * 180 / Constants::PI;
}