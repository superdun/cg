#pragma once
#include "Sphere.h"
#include "VectorHelper.h"
#include "Light.h"
#include "Camera.h"
#include <array>
#include <vector>
#include <tuple> 
class RayTraceRender
{
private:
	const Camera* camera;
	const std::vector<const Sphere*> sphereList;
	const std::vector<const Light*> lightList;
	std::tuple<double, const Sphere*> GetClosestIntersection(const std::array<double, 3>& oPoint, const std::array<double, 3>& directionVector, const double& tMin, const double& tMax);
	double GetLighteningScale(const std::array<double, 3>& surfacePoint, const std::array<double, 3>& normalVector, const double& specular);
	double DiffuseReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& normalVector);
	double SpecularReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& surfaceToCameraRay, const std::array<double, 3>& normalVector, const double& specular);
public:
	RayTraceRender(const std::vector<const Sphere*>& sphereList, const std::vector<const Light*>& lightList,const Camera* camera);
	
	std::array<int, 3> GetViewPointColor(const std::array<double,3>& oPoint, const std::array<double, 3>& directionVector, const double& tMin, const double& tMax);
};

