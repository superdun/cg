
#ifndef VECTORHELPER_H
#define VECTORHELPER_H

#include <array>
#include <cmath> 
#include "Plane.h"
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
    static std::array< std::array<double, 4>, 4> Build4DScaleMatrix(const double scale);
    static std::array< std::array<double, 4>, 4> Build4DScaleMatrix(const std::array<double, 3>& scale);
    static std::array< std::array<double, 4>, 4> Build4DRotationMatrix(const double angle, const std::array<double, 3>& axis);
    static std::array< std::array<double, 4>, 4> Build4DRotateInverseMatrix(const double angle, const std::array<double, 3>& axis);
    static std::array< std::array<double, 4>, 4> Build4DTranslationMatrix(const std::array<double, 3>& translation);
    static std::array< std::array<double, 4>, 4> Build4DInverseTranslationMatrix(const std::array<double, 3>& translation);
    static std::array< std::array<double, 4>, 4> Build4DProjectionViewportToCanvasMatrix(const double viewportDistance, const double canvasWidth, const double canvasHeight, const double viewportWidth, const double viewportHeight);
    static std::array<double, 4>  VerticeMatrixMultiply(const std::array< double, 4>& vertice, const std::array< std::array<double, 4>, 4>& matrix);
    static std::array< std::array<double, 4>, 4> MatrixMultiply(const std::array< std::array<double, 4>, 4>& matrix1, const std::array< std::array<double, 4>, 4>& matrix2);
    static std::array<double, 4> BuildHomogeneousPoint(const std::array<double, 3>& point);
    static std::array<double, 4> BuildHomogeneousDirection(const std::array<double, 3>& direction);
    static std::array<double, 3> Build3DPoint(const std::array<double, 4>& point);
    static std::array<double, 2> Build2DPoint(const std::array<double, 4>& point);
    static std::array<double, 3> Build2DPointWithDepth(const std::array<double, 4>& point);
    static double GetSingnedVertexToPlaneDistance(const std::array<double, 3>& vertex, const Plane* planePointer);
    static std::array<double, 3> GetPlaneNormal(const std::array<double, 4>* planePointer);
    static std::pair<std::array<double, 3>, double> GetPlaneLineIntersection(const Plane* plane, const std::array<double, 3>& pointA, const std::array<double, 3>& pointB);
    static std::array<double, 3> GetNormal(const std::array<double, 3>& p1, const std::array<double, 3>& p2, const std::array<double, 3>& p3);
    static std::array<double, 3> GetNormalWithoutNormalize(const std::array<double, 3>& p1, const std::array<double, 3>& p2, const std::array<double, 3>& p3);
    static double GetAngleBetweenVectors(const std::array<double, 3>& v1, const std::array<double, 3>& v2);
    static double GetCosBetweenVectors(const std::array<double, 3>& v1, const std::array<double, 3>& v2);
};

#endif // VECTORHELPER_H
