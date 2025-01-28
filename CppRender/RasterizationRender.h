#pragma once
#include <array>
#include <vector>
#include "Pixel.h"
#include "Camera.h"
#include "Model.h"
#include "Canvas.h"
#include "ModelInstance.h"
#include "Plane.h"
class RasterizationRender
{
private:
	const Camera* camera;
	Canvas* canvas;
	const std::vector<ModelInstance*>& instances;
	const std::vector<const Light*> originalLightList;
	std::vector<Light*> lightList;
	const std::array<Plane*, 5> planes;
	std::vector<std::vector<double>> depthBuffer;
	void ClipPipeline();
	ModelInstance* ClipInstance( ModelInstance* instance, const std::array< std::array<double, 4>, 4>& matrix_model_camera);
	ModelInstance* ClipInstanceAgainstPlane( ModelInstance* instance,  const Plane* plane, const std::array< std::array<double, 4>, 4>& matrix_model_camera);
	bool IsBackTriangle(const Triangle* triangle,const std::array< std::array<double, 4>, 4>& matrix_model_camera);
	std::vector<Triangle*> ClipTrianglesAgainstPlane(const ModelInstance* instance, const Plane* plane, const std::array< std::array<double, 4>, 4>& matrix_model_camera);
	std::vector<Triangle*>  ClipTriangleAgainstPlane(const Triangle* triangle, const Plane* plane, const std::array< std::array<double, 4>, 4>& matrix_model_camera);
	ModelInstance* CreateNewInstance(const ModelInstance* instance, const std::array< std::array<double, 4>, 4>& matrix_model_camera);
	void InitDepthBuffer();
	void ClearDepthBuffer();
	double GetLighteningScale(const std::array<double, 3>& surfacePoint, const std::array<double, 3>& normalVector, const double& specular) const;
	double DiffuseReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& normalVector) const;
	double SpecularReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& surfaceToCameraRay, const std::array<double, 3>& normalVector, const double& specular) const;
public:
	RasterizationRender(Canvas* canvas, const std::vector<ModelInstance*>& instances, const Camera* camera, const std::array<Plane*, 5>& planes, const std::vector<const Light*>& lightList);
	~RasterizationRender();
	std::vector<double> Interpolate(int i0, double d0, int i1, double d1) const;
	std::vector<Pixel*> DrawLine(const std::array<int, 2> p0, const std::array<int, 2> p1,const double p0_depth, const double p1_depth, const std::array<int, 3> color) const;
	std::vector<Pixel*> DrawWireFrameTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2,const double p0_depth, const double p1_depth, const double p2_depth, const std::array<int, 3>& color) const;
	std::vector<Pixel*> DrawFilledTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const double p0_depth, const double p1_depth, const double p2_depth, const std::array<int, 3>& color) const;
	std::vector<Pixel*> DrawShadedTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const double p0_depth, const double p1_depth, const double p2_depth, const double h1, const double h2, const double h3, const std::array<int, 3>& color) const;
	std::array<double, 2> ProjectVertex(const std::array<double, 3>& vertex, const double d, const std::array<double, 3>& modelPosition) const;
	std::vector<Pixel*> RenderInstance(const ModelInstance& model, const std::array< std::array<double, 4>, 4>& matrix_camera) const;
	void RunRender();			
	void SetCanvas(Canvas* canvas);
	bool CompareAndSetDepthBuffer(const std::array<int, 2>& point, const double depth);
	const std::vector<Light*> GetLightList() const;
};

