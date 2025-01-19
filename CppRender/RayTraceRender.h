#pragma once
#include "Sphere.h"
#include "VectorHelper.h"
#include "Light.h"
#include "Camera.h"
#include <array>
#include <vector>
#include <tuple> 
#include "Canvas.h"
class RayTraceRender
{
private:
	const Camera* camera;
	 Canvas* canvas;
	const std::vector<const Sphere*> sphereList;
	const std::vector<const Light*> lightList;
	std::tuple<double, const Sphere*> GetClosestIntersection(const std::array<double, 3>& oPoint, const std::array<double, 3>& directionVector, const double& tMin, const double& tMax) const;
	double GetLighteningScale(const std::array<double, 3>& surfacePoint, const std::array<double, 3>& normalVector, const double& specular) const;
	double DiffuseReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& normalVector) const;
	double SpecularReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& surfaceToCameraRay, const std::array<double, 3>& normalVector, const double& specular) const;
	void RenderSection(int startY, int endY, std::vector<std::vector<COLORREF>>& buffer) const;
	void MergeBuffer(std::vector<std::vector<COLORREF>>& buffer) const;
public:
	RayTraceRender( Canvas* canvas,const std::vector<const Sphere*>& sphereList, const std::vector<const Light*>& lightList,const Camera* camera );
	~RayTraceRender();
	void RunRender() const;
	void SetCanvas( Canvas* canvas);
	std::array<int, 3> GetViewPointColor(const std::array<double,3>& oPoint, const std::array<double, 3>& directionVector, const double& tMin, const double& tMax, const int& depth) const;
};

