#include "VectorHelper.h"
#include "Constants.h"

std::array<double, 3> VectorHelper::VectorAdd(const std::array<double, 3>& point, const std::array<double, 3>& v) {
    std::array<double, 3> result;
    result[0] = point[0] + v[0];
    result[1] = point[1] + v[1];
    result[2] = point[2] + v[2];
    return result;
}

std::array<int, 3> VectorHelper::ColorVectorAdd(const std::array<int, 3>& v1, const std::array<int, 3>& v2)
{
    std::array<int, 3> result;
    result[0] = std::min(v1[0] + v2[0], 255);
    result[1] = std::min(v1[1] + v2[1], 255);
    result[2] = std::min(v1[2] + v2[2], 255);
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

std::array<int, 3> VectorHelper::ColorVectorScale(const std::array<int, 3>&v, double scale) {
    std::array<int, 3> result;
    result[0] = std::min(static_cast<int>( v[0] * scale),255);
    result[1] = std::min(static_cast<int>(v[1] * scale), 255);
    result[2] = std::min(static_cast<int>(v[2] * scale), 255);
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

std::array<double, 3> VectorHelper::GetReflectVector(const std::array<double, 3>& originRay, const std::array<double, 3>& normal)
{

    return VectorHelper::VectorSub(VectorHelper::VectorScale(normal, 2 * VectorHelper::VectorDot(normal, originRay)), originRay); ;
}

std::array<double, 3> VectorHelper::VecRotate(const std::array<double, 3>& vector, double angleWithAxisX, double angleWithAxisY, double angleWithAxisZ)
{
    std::array<double, 3> result;

    // Rotate around Z-axis
    double xWithAxisZ = vector[0] * cos(angleWithAxisZ) - vector[1] * sin(angleWithAxisZ);
    double yWithAxisZ = vector[0] * sin(angleWithAxisZ) + vector[1] * cos(angleWithAxisZ);
    double zWithAxisZ = vector[2];

    // Rotate around Y-axis
    double xWithAxisY = xWithAxisZ * cos(angleWithAxisY) - zWithAxisZ * sin(angleWithAxisY);
    double yWithAxisY = yWithAxisZ; // unchanged
    double zWithAxisY = xWithAxisZ * sin(angleWithAxisY) + zWithAxisZ * cos(angleWithAxisY);

    // Rotate around X-axis
    double xWithAxisX = xWithAxisY; // unchanged
    double yWithAxisX = zWithAxisY * sin(angleWithAxisX) + yWithAxisY * cos(angleWithAxisX);
    double zWithAxisX = zWithAxisY * cos(angleWithAxisX) - yWithAxisY * sin(angleWithAxisX);

    result[0] = xWithAxisX;
    result[1] = yWithAxisX;
    result[2] = zWithAxisX;

    return result;
}

