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
    return std::array<double, 3>();
}
std::array<std::array<double, 4>, 4> VectorHelper::BuildModelCompositeMatrix(
    const std::array<double, 3>& scale,
    const std::array<double, 3>& angleWithAxis,
    const std::array<double, 3>& translate,
    const double d)
{
    // 提取缩放
    double sx = scale[0];
    double sy = scale[1];
    double sz = scale[2];

    // 提取旋转角度
    double theta_x = angleWithAxis[0]; // 绕 x 轴
    double theta_y = angleWithAxis[1]; // 绕 y 轴
    double theta_z = angleWithAxis[2]; // 绕 z 轴

    // 计算三角函数
    double cos_x = std::cos(theta_x);
    double sin_x = std::sin(theta_x);
    double cos_y = std::cos(theta_y);
    double sin_y = std::sin(theta_y);
    double cos_z = std::cos(theta_z);
    double sin_z = std::sin(theta_z);

    // 构建组合变换矩阵 M
    std::array<std::array<double, 4>, 4> M = { {
        { (sx * cos_y * cos_z) / d, (-sy * cos_y * sin_z) / d, sz * sin_y, translate[0] },
        { (sx * sin_z * cos_x) / d, (sy * cos_z * cos_x) / d, (-sz * sin_x) / d, translate[1] },
        { (-sin_y * cos_z + sin_x * sin_y * sin_z) / d,
          (sin_y * sin_z + sin_x * cos_y * cos_z) / d,
          cos_y * cos_x, translate[2] },
        { 0, 0, 0, 1 }
    } };

    return M;
}

std::array<std::array<double, 4>, 4> VectorHelper::BuildCameraCompositeMatrix(
    const std::array<double, 3>& angleWithAxis,
    const std::array<double, 3>& translate,
    const double d)
{
    double dx = translate[0];
    double dy = translate[1];
    double dz = translate[2];

    double theta_x = angleWithAxis[0];
    double theta_y = angleWithAxis[1];
    double theta_z = angleWithAxis[2];

    double cos_x = std::cos(theta_x);
    double sin_x = std::sin(theta_x);
    double cos_y = std::cos(theta_y);
    double sin_y = std::sin(theta_y);
    double cos_z = std::cos(theta_z);
    double sin_z = std::sin(theta_z);

    // 最终组合矩阵 M
    std::array<std::array<double, 4>, 4> M_final = { {
        {cos_y * cos_z, cos_y * sin_z, -sin_y, -dx},
        {-sin_x * sin_y * cos_z + sin_z * cos_x, -sin_x * sin_y * sin_z - cos_z * cos_x, sin_x * cos_y, -dy},
        {sin_x * sin_y * sin_z + sin_y * cos_x, sin_x * sin_y * cos_z - sin_z * cos_x, cos_y * cos_x, -dz},
        {0, 0, 0, 1}
    } };

    return M_final; 
}

