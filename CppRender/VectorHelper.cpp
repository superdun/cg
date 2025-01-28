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

std::array<int, 3> VectorHelper::ColorVectorScale(const std::array<int, 3>& v, double scale) {
    std::array<int, 3> result;
    result[0] = std::min(static_cast<int>(v[0] * scale), 255);
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

std::array< std::array<double, 4>, 4> VectorHelper::Build4DScaleMatrix(const double scale)
{
    std::array< std::array<double, 4>, 4> matrix = { {
        {scale,0,0,0},
        {0,scale,0,0},
        {0,0,scale,0},
        {0,0,0,1}
    } };
    return matrix;
}

std::array< std::array<double, 4>, 4> VectorHelper::Build4DScaleMatrix(const std::array<double, 3>& scale)
{
    std::array< std::array<double, 4>, 4> matrix = { {
        {scale[0],0,0,0},
        {0,scale[1],0,0},
        {0,0,scale[2],0},
        {0,0,0,1}
    } };
    return matrix;
}

std::array< std::array<double, 4>, 4> VectorHelper::Build4DRotationMatrix(const double angle, const std::array<double, 3>& axis)
{
    double c = cos(angle);
    double s = sin(angle);
    double x = axis[0];
    double y = axis[1];
    double z = axis[2];
    std::array<std::array<double, 4>, 4> matrix = { {
        {c + x * x * (1 - c),x * y * (1 - c) + z * s,x * z * (1 - c) - y * s,0},
        {x * y * (1 - c) - z * s,c + y * y * (1 - c),y * z * (1 - c) + x * s,0},
        {z * x * (1 - c) + y * s,z * y * (1 - c) - x * s,c + z * z * (1 - c),0},
        {0,0,0,1}
    } };
    return matrix;

}

std::array< std::array<double, 4>, 4> VectorHelper::Build4DRotateInverseMatrix(const double angle, const std::array<double, 3>& axis)
{
    double c = cos(angle);
    double s = sin(angle);
    double x = axis[0];
    double y = axis[1];
    double z = axis[2];

    std::array<std::array<double, 4>, 4> matrix = { {
        {c + x * x * (1 - c),x * y * (1 - c) - z * s,x * z * (1 - c) + y * s,0},
        {x * y * (1 - c) + z * s,c + y * y * (1 - c),y * z * (1 - c) - x * s,0},
        {z * x * (1 - c) - y * s,z * y * (1 - c) + x * s,c + z * z * (1 - c),0},
        {0,0,0,1}
        } };
    return matrix;

}

std::array< std::array<double, 4>, 4> VectorHelper::Build4DTranslationMatrix(const std::array<double, 3>& translation)
{
    std::array<std::array<double, 4>, 4> matrix = { {
            {1,0,0,0},
            {0,1,0,0},
            {0,0,1,0},
            {translation[0],translation[1],translation[2],1}
    } };
    return matrix;
}

std::array< std::array<double, 4>, 4> VectorHelper::Build4DInverseTranslationMatrix(const std::array<double, 3>& translation)
{
    std::array<std::array<double, 4>, 4> matrix = { {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {-translation[0], -translation[1], -translation[2], 1}
        } };
    return matrix;
}

std::array<double, 4> VectorHelper::VerticeMatrixMultiply(const std::array< double, 4>& vertice, const std::array< std::array<double, 4>, 4>& matrix)
{
    std::array<double, 4> result;
    for (int i = 0; i < 4; i++) {
        result[i] = vertice[0] * matrix[0][i] + vertice[1] * matrix[1][i] + vertice[2] * matrix[2][i] + vertice[3] * matrix[3][i];
    }
    return result;
}

std::array< std::array<double, 4>, 4> VectorHelper::MatrixMultiply(const std::array< std::array<double, 4>, 4>& matrix1, const std::array< std::array<double, 4>, 4>& matrix2)
{
    std::array< std::array<double, 4>, 4> result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = matrix1[i][0] * matrix2[0][j] + matrix1[i][1] * matrix2[1][j] + matrix1[i][2] * matrix2[2][j] + matrix1[i][3] * matrix2[3][j];
        }
    }
    return result;
}

std::array< std::array<double, 4>, 4> VectorHelper::Build4DProjectionViewportToCanvasMatrix(const double viewportDistance, const double canvasWidth, const double canvasHeight, const double viewportWidth, const double viewportHeight)
{
    std::array< std::array<double, 4>, 4> matrix = { {
        {viewportDistance * canvasWidth / viewportWidth,0,0,0},
        {0,viewportDistance * canvasHeight / viewportHeight,0,0},
        {0,0,1,0},
        {0,0,0,1}
    } };
    return matrix;
}
std::array<double, 4> VectorHelper::BuildHomogeneousPoint(const std::array<double, 3>& point)
{
    std::array<double, 4> result = { point[0],point[1],point[2],1 };
    return result;
}

std::array<double, 4> VectorHelper::BuildHomogeneousDirection(const std::array<double, 3>& direction)
{
    std::array<double, 4> result = { direction[0],direction[1],direction[2],0 };
    return result;
}

std::array<double, 3> VectorHelper::Build3DPoint(const std::array<double, 4>& point)
{
    std::array<double, 3> result = { point[0] ,point[1] ,point[2] };
    return result;
}

std::array<double, 2> VectorHelper::Build2DPoint(const std::array<double, 4>& point)
{
    std::array<double, 2> result = { point[0] / point[2],point[1] / point[2] };
    return result;
}

std::array<double, 3> VectorHelper::Build2DPointWithDepth(const std::array<double, 4>& point)
{
    std::array<double, 3> result = { point[0] / point[2],point[1] / point[2],point[2] };
    return result;
}

double VectorHelper::GetSingnedVertexToPlaneDistance(const std::array<double, 3>& vertex, const Plane* plane)
{
    const std::array<double, 3> normal = plane->GetNormal();
    const double d = plane->GetD();
    return (vertex[0] * normal[0] + vertex[1] * normal[1] + vertex[2] * normal[2] + d);
}
std::array<double, 3> VectorHelper::GetPlaneNormal(const std::array<double, 4>* planePointer)
{
    const auto plane = *planePointer;
    const double normalLength = sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
    return std::array<double, 3>{plane[0] / normalLength, plane[1] / normalLength, plane[2] / normalLength};
}

std::pair<std::array<double, 3>, double> VectorHelper::GetPlaneLineIntersection(const Plane* planePointer, const std::array<double, 3>& pointA, const std::array<double, 3>& pointB)
{
    const double D = planePointer->GetD();
    const auto normal = planePointer->GetNormal();
    const double t = -1 * D - VectorDot(normal, pointA) / VectorDot(normal, VectorSub(pointB, pointA));
    return { VectorAdd(pointA, VectorScale(VectorSub(pointB, pointA), t)), t };
}

std::array<double, 3> VectorHelper::GetNormal(const std::array<double, 3>& p1, const std::array<double, 3>& p2, const std::array<double, 3>& p3)
{
    return VectorNormalize(GetNormalWithoutNormalize(p1, p2, p3));
}

std::array<double, 3> VectorHelper::GetNormalWithoutNormalize(const std::array<double, 3>& p1, const std::array<double, 3>& p2, const std::array<double, 3>& p3)
{
    return VectorProduct(VectorSub(p2, p1), VectorSub(p3, p1));
}

double VectorHelper::GetAngleBetweenVectors(const std::array<double, 3>& v1, const std::array<double, 3>& v2)
{
    return acos(GetCosBetweenVectors(v1, v2));
}

double VectorHelper::GetCosBetweenVectors(const std::array<double, 3>& v1, const std::array<double, 3>& v2)
{
    return VectorDot(v1, v2) / (VectorLength(v1) * VectorLength(v2));
}

std::array<double, 3> VectorHelper::GetCentroid(const std::array<double, 3>& p1, const std::array<double, 3>& p2, const std::array<double, 3>& p3)
{
    return VectorScale(VectorAdd(VectorAdd(p1, p2), p3), 1.0 / 3.0);
}

