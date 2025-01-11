#include "RayTraceRender.h"
#include <algorithm> 
#include <tuple>

double RayTraceRender::GetLighteningScale(const std::array<double, 3>& surfacePoint, const std::array<double, 3>& normalVector, const double& specular)
{
    const std::array<double, 3> surfaceToCameraVector = VectorHelper::VectorSub(camera->GetPosition(), surfacePoint);
    double lightIntensityScale = 0;

    for (size_t i = 0; i < lightList.size(); i++)
    {
        const Light* currentLight = lightList[i];
        std::array<double, 3> lightDirectionFromSurface;
        double tMax = 0; 
        double minT = 0; 
        const Sphere* closestSphere = nullptr; 

        switch (currentLight->getLightType())
        {
        case LightTypeEnum::AmbientLight:
            lightIntensityScale += currentLight->getIntensity();
            break;

        case LightTypeEnum::DirectionalLight:
        case LightTypeEnum::PointLight:
            if (currentLight->getLightType() == LightTypeEnum::PointLight)
            {
                lightDirectionFromSurface = VectorHelper::VectorSub(currentLight->getPosition(), surfacePoint);
                tMax = 1; 
            }
            else 
            {
                lightDirectionFromSurface = currentLight->getDirection();
                tMax = Constants::Infinity; 
            }

            {
                auto [minTTemp, closestSphereTemp] = GetClosestIntersection(surfacePoint, lightDirectionFromSurface, Constants::Infinitesimal, tMax);
                minT = minTTemp; 
                closestSphere = closestSphereTemp; 
            }

            if (closestSphere != nullptr)
            {
                lightIntensityScale += currentLight->getIntensity() * DiffuseReflectionScale(lightDirectionFromSurface, normalVector);
                lightIntensityScale += currentLight->getIntensity() * SpecularReflectionScale(lightDirectionFromSurface, surfaceToCameraVector, normalVector, specular);
            }
            break;

        default:
            break;
        }
    }
    return lightIntensityScale;
}
double RayTraceRender::DiffuseReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& normalVector)
{

	return  std::max(0.0, VectorHelper::VectorDot(normalVector, originRay) / VectorHelper::VectorLength(originRay)) ;

}
double RayTraceRender::SpecularReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& surfaceToCameraRay, const std::array<double, 3>& normalVector, const double& specular)
{
	const std::array<double, 3> reflectRay = VectorHelper::GetReflectVector(originRay, normalVector);
	return  std::max(0.0, VectorHelper::VectorDot(reflectRay, surfaceToCameraRay) / (VectorHelper::VectorLength(reflectRay) * VectorHelper::VectorLength(surfaceToCameraRay)));
}

RayTraceRender::RayTraceRender(const std::vector<const Sphere*>& sphereList, const std::vector<const Light*>& lightList, const Camera* camera)
	:sphereList(sphereList),lightList(lightList),camera(camera)
{

}

std::array<int, 3> RayTraceRender::GetViewPointColor(const std::array<double, 3>& oPoint, const std::array<double, 3>& directionVector, const double& tMin, const double& tMax)
{
	auto [minT, closestSphere] = GetClosestIntersection(oPoint, directionVector, tMin, tMax);

	if (closestSphere == nullptr)
	{
		return Constants::BACKGROUND_COLOR;
	}
	
	const std::array<double, 3> surfacePoint = VectorHelper::VectorAdd(oPoint, VectorHelper::VectorScale(directionVector, minT));
	const std::array<double, 3> normalVector = VectorHelper::VectorNormalize(VectorHelper::VectorSub(surfacePoint, closestSphere->GetCenterPoint())) ;
    double intensityScale = 1;
    //double intensityScale = GetLighteningScale(surfacePoint, normalVector,closestSphere->GetSpecular());
	return VectorHelper::VectorScale(closestSphere->GetColor(), intensityScale);
}

std::tuple<double, const Sphere*> RayTraceRender::GetClosestIntersection(const std::array<double, 3>& oPoint, const std::array<double, 3>& directionVector, const double& tMin, const double& tMax)
{
	double minT = Constants::Infinity;
	const Sphere* closestSpere = nullptr;
	for (size_t i = 0; i < sphereList.size(); i++)
	{
		const Sphere* currentSphere = sphereList[i];

		double currentMinT = currentSphere->GetIntersectT(oPoint, directionVector);
		if (currentMinT <= tMin || currentMinT >= tMax) {
			continue;
		}
		if (currentMinT <= minT) {
			closestSpere = currentSphere;
			minT = currentMinT;
		}
	}
	return  std::make_tuple(minT, closestSpere);
}

