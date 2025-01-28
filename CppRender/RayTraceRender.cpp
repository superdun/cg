#define NOMINMAX
#include "RayTraceRender.h"
#include <algorithm> 
#include <tuple>
#include <iostream>
#include <thread>

double RayTraceRender::GetLighteningScale(const std::array<double, 3>& surfacePoint, const std::array<double, 3>& normalVector, const double& specular) const
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

        switch (currentLight->GetLightType())
        {
        case LightTypeEnum::AmbientLight:
            lightIntensityScale += currentLight->GetIntensity();
            break;

        case LightTypeEnum::DirectionalLight:
        case LightTypeEnum::PointLight:
            if (currentLight->GetLightType() == LightTypeEnum::PointLight)
            {
                lightDirectionFromSurface = VectorHelper::VectorSub(currentLight->GetPosition(), surfacePoint);
                tMax = 100; 
            }
            else 
            {
                lightDirectionFromSurface = currentLight->GetDirection();
                tMax = Constants::Infinity; 
            }

            {
                auto [minTTemp, closestSphereTemp] = GetClosestIntersection(surfacePoint, lightDirectionFromSurface, Constants::Infinitesimal, tMax);
                minT = minTTemp; 
                closestSphere = closestSphereTemp; 
            }

            if (closestSphere == nullptr)
            {
                lightIntensityScale += currentLight->GetIntensity() * DiffuseReflectionScale(lightDirectionFromSurface, normalVector);
                lightIntensityScale += currentLight->GetIntensity() * SpecularReflectionScale(lightDirectionFromSurface, surfaceToCameraVector, normalVector, specular);
            }
            break;

        default:
            break;
        }
    }
    return lightIntensityScale;
}
double RayTraceRender::DiffuseReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& normalVector) const
{

	return  std::max(0.0, VectorHelper::VectorDot(normalVector, originRay) / VectorHelper::VectorLength(originRay)) ;

}
double RayTraceRender::SpecularReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& surfaceToCameraRay, const std::array<double, 3>& normalVector, const double& specular) const
{
	const std::array<double, 3> reflectRay = VectorHelper::GetReflectVector(originRay, normalVector);
	return  std::max(0.0, std::pow(VectorHelper::VectorDot(reflectRay, surfaceToCameraRay) / (VectorHelper::VectorLength(reflectRay) * VectorHelper::VectorLength(surfaceToCameraRay)),specular));
}

void RayTraceRender::RenderSection(int startY, int endY, std::vector<std::vector<COLORREF>>& buffer) const
{
    for (int x = 0; x <  canvas->getCanvasWidth(); x++)
    {
        for (int y = startY; y < endY; y++)
        {
            double viewportX = canvas->ConvertXToViewportCoordinate(x);
            double viewportY = canvas->ConvertYToViewportCoordinate(y);

            std::array<double, 3> directionVector = VectorHelper::VectorSub({ viewportX,viewportY,canvas->getViewportDistance()}, {0,0,0});
            directionVector = VectorHelper::VecRotate(directionVector, 0, camera->GetAngle(), 0);
            auto colorArray = GetViewPointColor(camera->GetPosition(), directionVector, 1, Constants::Infinity, 1);
            COLORREF colorPixelRef = RGB(colorArray[0], colorArray[1], colorArray[2]);
            buffer[y][x] = colorPixelRef;
        }
    }
}

void RayTraceRender::MergeBuffer(std::vector<std::vector<COLORREF>>& buffer) const
{
    for (int y = 0; y < canvas->getCanvasHeight(); y++)
    {
        for (int x = 0; x < canvas->getCanvasWidth(); x++)
        {
            canvas->PutPixel( x, y, buffer[y][x]);
        }
    }
}

RayTraceRender::RayTraceRender( Canvas* canvas, const std::vector<const Sphere*>& sphereList, const std::vector<const Light*>& lightList, const Camera* camera)
	:canvas(canvas), sphereList(sphereList),lightList(lightList),camera(camera)
{
   

}

RayTraceRender::~RayTraceRender()
{
}


void RayTraceRender::RunRender() const
{
    const int numThreads = std::thread::hardware_concurrency(); 
    std::vector<std::thread> threads;
    std::vector<std::vector<COLORREF>> buffer(canvas->getCanvasHeight(), std::vector<COLORREF>(canvas->getCanvasWidth()));

    int sectionHeight = canvas->getCanvasHeight() / numThreads;
    for (int i = 0; i < numThreads; ++i)
    {
        int startY = i * sectionHeight;
        int endY = (i == numThreads - 1) ? canvas->getCanvasHeight() : startY + sectionHeight; 
        threads.emplace_back(&RayTraceRender::RenderSection, this, startY, endY, std::ref(buffer));
    }


    for (auto& thread : threads)
    {
        thread.join();
    }

    MergeBuffer(buffer);
}

void RayTraceRender::SetCanvas(Canvas* canvasPointer)
{
    canvas = canvasPointer;
}


std::array<int, 3> RayTraceRender::GetViewPointColor(const std::array<double, 3>& oPoint, const std::array<double, 3>& directionVector, const double& tMin, const double& tMax, const int& depth) const
{
	auto [minT, closestSphere] = GetClosestIntersection(oPoint, directionVector, tMin, tMax);

	if (closestSphere == nullptr)
	{
		return Constants::BACKGROUND_COLOR;
	}
	
	const std::array<double, 3> surfacePoint = VectorHelper::VectorAdd(oPoint, VectorHelper::VectorScale(directionVector, minT));
	const std::array<double, 3> normalVector = VectorHelper::VectorNormalize(VectorHelper::VectorSub(surfacePoint, closestSphere->GetCenterPoint())) ;
    double intensityScale = GetLighteningScale(surfacePoint, normalVector,closestSphere->GetSpecular()) ;
    const std::array<int, 3> localColor = VectorHelper::ColorVectorScale(closestSphere->GetColor(), intensityScale);
    if (depth==Constants::Max_Depth)
    {
        return localColor;
    }
    else
    {
        const std::array<double, 3> reflectRay = VectorHelper::GetReflectVector(VectorHelper::VectorScale(directionVector, -1), normalVector);
        const std::array<int, 3> reflectedColor = GetViewPointColor(surfacePoint, reflectRay, Constants::Infinitesimal, Constants::Infinity, depth + 1);
        const std::array<int, 3> finalColor = VectorHelper::ColorVectorScale(localColor, 1 - closestSphere->GetReflective());
        const std::array<int, 3> reflectedColorScaled = VectorHelper::ColorVectorScale(reflectedColor, closestSphere->GetReflective());
        return VectorHelper::ColorVectorAdd(finalColor, reflectedColorScaled);
    }
}

std::tuple<double, const Sphere*> RayTraceRender::GetClosestIntersection(const std::array<double, 3>& oPoint, const std::array<double, 3>& directionVector, const double& tMin, const double& tMax) const
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

