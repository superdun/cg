#define NOMINMAX
#include "RasterizationRender.h"
#include <algorithm>
#include "VectorHelper.h"
#include <unordered_map>
#include "Utils.h"
#include <future>


RasterizationRender::RasterizationRender(Canvas *canvas, const std::vector<ModelInstance *> &instances, const Camera *camera, const std::array<Plane *, 5> &planes, const std::vector<const Light*>& lightList)
    : instances(instances), canvas(canvas), camera(camera), planes(planes), originalLightList(lightList)
{
	InitDepthBuffer();
}


RasterizationRender::~RasterizationRender()
{
	for (auto& light : lightList)
	{
		delete light;
	}
}

void RasterizationRender::InitDepthBuffer()
{
	if (canvas != nullptr)
	{
		depthBuffer = std::vector<std::vector<double>>(canvas->getCanvasWidth()+1, std::vector<double>(canvas->getCanvasHeight()+1, 0));
		for (int i = 0; i < canvas->getCanvasWidth(); i++)
		{
			for (int j = 0; j < canvas->getCanvasHeight(); j++)
			{
				depthBuffer[i][j] = 0;
			}
		}
	}
}

void RasterizationRender::ClearDepthBuffer()
{
	InitDepthBuffer();
}

double RasterizationRender::GetLighteningScale(const std::array<double, 3>& surfacePoint, const std::array<double, 3>& normalVector, const double& specular) const
{
	//书上此处代码有错，不应从摄像机当前位置计算，因为已经经过了坐标变换
	const std::array<double, 3> surfaceToCameraVector = VectorHelper::VectorSub({0,0,0}, surfacePoint);
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

			lightIntensityScale += currentLight->GetIntensity() * DiffuseReflectionScale(lightDirectionFromSurface, normalVector);
            lightIntensityScale += currentLight->GetIntensity() * SpecularReflectionScale(lightDirectionFromSurface, surfaceToCameraVector, normalVector, specular);
            break;

        default:
            break;
        }
    }
    return lightIntensityScale;
}
double RasterizationRender::DiffuseReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& normalVector) const
{

	return  std::max(0.0, VectorHelper::VectorDot(normalVector, originRay) / VectorHelper::VectorLength(originRay)) ;

}
double RasterizationRender::SpecularReflectionScale(const std::array<double, 3>& originRay, const std::array<double, 3>& surfaceToCameraRay, const std::array<double, 3>& normalVector, const double& specular) const
{
	//书上此处代码有错，计算反射光使用单位法向量
	const std::array<double, 3> reflectRay = VectorHelper::GetReflectVector(originRay, normalVector);
	const auto cos = VectorHelper::GetCosBetweenVectors(reflectRay, surfaceToCameraRay);
	auto pw = std::pow(cos, specular);
	return  std::max(0.0, pw);
}
std::vector<double>  RasterizationRender::Interpolate(int i0, double d0, int i1, double d1) const
{
	std::vector<double> result;
	if (i0 == i1)
	{
		result.push_back(d0);
		return result;
	}
	const double a = (d1 - d0) / (i1 - i0);
	double d = d0;
	for (int i = i0; i <= i1; i++)
	{
		result.push_back(d);
		d += a;
	}
	return result;
}
 
std::vector<Pixel*> RasterizationRender::DrawLine(const std::array<int, 2> p0, const std::array<int, 2> p1,const double p0_depth, const double p1_depth, const std::array<int, 3> color) const
{
	std::array<int, 3> p0_new = { p0[0],p0[1],0 };
	std::array<int, 3> p1_new = { p1[0],p1[1],0 };
	std::vector<Pixel*> result;
	const bool steep = abs(p0_new[1] - p1_new[1]) > abs(p0_new[0] - p1_new[0]);
	if (steep)
	{
		
		if (p0_new[1]> p1_new[1])
		{
			std::swap(p0_new, p1_new);
		}
		auto xList = Interpolate(p0_new[1], p0_new[0], p1_new[1], p1_new[0]);
		for (int p = p0_new[1]; p <= p1_new[1]; p++)
		{
			result.push_back(new Pixel(xList[p - p0_new[1]], p, color, p0_new[2]));
		}
		
	}
	else {
		if (p0_new[0] > p1_new[0])
		{
			std::swap(p0_new, p1_new);
		}
		auto yList = Interpolate(p0_new[0], p0_new[1], p1_new[0], p1_new[1]);
		for (int p = p0_new[0]; p <= p1_new[0]; p++)
		{
			result.push_back(new Pixel(p, yList[p - p0_new[0]], color, p0_new[2]));
		}
	}

	return result;
}

std::vector<Pixel*> RasterizationRender::DrawWireFrameTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2,const double p0_depth, const double p1_depth, const double p2_depth, const std::array<int, 3>& color) const
{


	std::vector<Pixel*> result;
	auto line1 = DrawLine(p0, p1, p0_depth, p1_depth, color);
	auto line2 = DrawLine(p1, p2, p1_depth, p2_depth, color);
	auto line3 = DrawLine(p0, p2, p0_depth, p2_depth, color);
	// Insert the resulting pixels from each line into the result vector
	result.insert(result.end(), line1.begin(), line1.end());
	result.insert(result.end(), line2.begin(), line2.end());
	result.insert(result.end(), line3.begin(), line3.end());
	return result;
}

std::vector<Pixel*> RasterizationRender::DrawFilledTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const double p0_depth, const double p1_depth, const double p2_depth, const std::array<int, 3>& color) const
{
	std::vector<Pixel*> result;
	std::array<int, 2> p0_new = p0;
	std::array<int, 2> p1_new = p1;
	std::array<int, 2> p2_new = p2;
	double p0_depth_new = p0_depth;	
	double p1_depth_new = p1_depth;
	double p2_depth_new = p2_depth;
	if (p0_new[1]>p1_new[1])
	{
		std::swap(p0_new, p1_new);
		std::swap(p0_depth_new, p1_depth_new);
	}
	if (p0_new[1] > p2_new[1])
	{
		std::swap(p0_new, p2_new);
		std::swap(p0_depth_new, p2_depth_new);
	}
	if (p1_new[1] > p2_new[1])
	{
		std::swap(p1_new, p2_new);
		std::swap(p1_depth_new, p2_depth_new);
	}
	auto xListLow = Interpolate(p0_new[1], p0_new[0], p2_new[1], p2_new[0]);
	auto xListHigher = Interpolate(p1_new[1], p1_new[0], p2_new[1], p2_new[0]);
	auto xListHighest = Interpolate(p0_new[1], p0_new[0], p1_new[1], p1_new[0]);
	
	auto reciprocalDepthListLow = Interpolate(p0_new[1], 1/p0_depth_new, p2_new[1], 1/p2_depth_new);
	auto reciprocalDepthListHigher = Interpolate(p1_new[1], 1/p1_depth_new, p2_new[1], 1/p2_depth_new);
	auto reciprocalDepthListHighest = Interpolate(p0_new[1], 1/p0_depth_new, p1_new[1], 1/p1_depth_new);

	for (int y = p0_new[1]; y <= p2_new[1]; y++)
	{
		auto xLow = xListLow[y - p0_new[1]];
		auto reciprocalDepthOnEdge1 = reciprocalDepthListLow[y - p0_new[1]];
		std::array<int, 2> pOnEdge1 = { xLow,y };
		double reciprocalDepthOnEdge2;
		std::array<int, 2> pOnEdge2;

		if (y< p1_new[1])
		{	
			reciprocalDepthOnEdge2 = reciprocalDepthListHighest[y - p0_new[1]];
			pOnEdge2 = { (int)xListHighest[y- p0_new[1]],y};

		}
		else {
			reciprocalDepthOnEdge2 = reciprocalDepthListHigher[y - p1_new[1]];
			pOnEdge2 = { (int)xListHigher[y - p1_new[1]],y };
		}
		if (pOnEdge1[0]> pOnEdge2[0])
		{
			std::swap(pOnEdge1, pOnEdge2);
			std::swap(reciprocalDepthOnEdge1, reciprocalDepthOnEdge2);
		}
		auto reciprocalDepthListHorizontal = Interpolate(pOnEdge1[0], reciprocalDepthOnEdge1, pOnEdge2[0], reciprocalDepthOnEdge2);
		for (int x = pOnEdge1[0]; x <= pOnEdge2[0]; x++)
		{
			auto reciprocalDepth = reciprocalDepthListHorizontal[x - pOnEdge1[0]];
			result.push_back(new Pixel(x,y,color,reciprocalDepth));
		}

	}


	return result;
}

std::vector<Pixel*> RasterizationRender::DrawShadedTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2,
const double p0_depth, const double p1_depth, const double p2_depth, 
const double h0, const double h1, const double h2, 
 const std::array<int, 3>& color) const
{
	std::vector<Pixel*> result;
	std::array<int, 2> p0_new = p0;
	std::array<int, 2> p1_new = p1;
	std::array<int, 2> p2_new = p2;
	double p0_depth_new = p0_depth;
	double p1_depth_new = p1_depth;
	double p2_depth_new = p2_depth;
	double h0_new = h0;
	double h1_new = h1;
	double h2_new = h2;

	if (p0_new[1] > p1_new[1])
	{
		std::swap(p0_new, p1_new);
		std::swap(h0_new, h1_new);
		std::swap(p0_depth_new, p1_depth_new);
	}
	if (p0_new[1] > p2_new[1])
	{
		std::swap(p0_new, p2_new);
		std::swap(h0_new, h2_new);
		std::swap(p0_depth_new, p2_depth_new);
	}
	if (p1_new[1] > p2_new[1])
	{
		std::swap(p1_new, p2_new);
		std::swap(h1_new, h2_new);
		std::swap(p1_depth_new, p2_depth_new);
	}
	auto xListLow = Interpolate(p0_new[1], p0_new[0], p2_new[1], p2_new[0]);
	auto hListLow = Interpolate(p0_new[1], h0_new, p2_new[1], h2_new);
	auto reciprocalDepthListLow = Interpolate(p0_new[1], 1/p0_depth_new, p2_new[1], 1/p2_depth_new);

	auto xListHigher = Interpolate(p1_new[1], p1_new[0], p2_new[1], p2_new[0]);
	auto hListHigher = Interpolate(p1_new[1], h1_new, p2_new[1], h2_new);
	auto reciprocalDepthListHigher = Interpolate(p1_new[1], 1/p1_depth_new, p2_new[1], 1/p2_depth_new);

	auto xListHighest = Interpolate(p0_new[1], p0_new[0], p1_new[1], p1_new[0]);
	auto hListHighest = Interpolate(p0_new[1], h0_new, p1_new[1], h1_new);
	auto reciprocalDepthListHighest = Interpolate(p0_new[1], 1/p0_depth_new, p1_new[1], 1/p1_depth_new);

	for (int y = p0_new[1]; y <= p2_new[1]; y++)
	{
		auto xOnEdge1 = xListLow[y - p0_new[1]];
		auto hOnEdge1 = hListLow[y - p0_new[1]];
		auto reciprocalDepthOnEdge1 = reciprocalDepthListLow[y - p0_new[1]];
		double hOnEdge2;
		double reciprocalDepthOnEdge2;
		std::array<int, 2> pOnEdge1 = { xOnEdge1,y };
		std::array<int, 2> pOnEdge2;
		if (y < p1_new[1])
		{
			hOnEdge2 = hListHighest[y - p0_new[1]];
			pOnEdge2 = { (int)xListHighest[y - p0_new[1]],y };
			reciprocalDepthOnEdge2 = reciprocalDepthListHighest[y - p0_new[1]];
		}
		else {
			hOnEdge2 = hListHigher[y - p1_new[1]];
			pOnEdge2 = { (int)xListHigher[y - p1_new[1]],y };
			reciprocalDepthOnEdge2 = reciprocalDepthListHigher[y - p1_new[1]];
		}
		if (pOnEdge1[0] > pOnEdge2[0])
		{
			std::swap(pOnEdge1, pOnEdge2);
			std::swap(hOnEdge1, hOnEdge2);
			std::swap(reciprocalDepthOnEdge1, reciprocalDepthOnEdge2);
		}
		auto hListHorizontal = Interpolate(pOnEdge1[0], hOnEdge1, pOnEdge2[0], hOnEdge2);
		auto reciprocalDepthListHorizontal = Interpolate(pOnEdge1[0], reciprocalDepthOnEdge1, pOnEdge2[0], reciprocalDepthOnEdge2);
		for (int x = pOnEdge1[0]; x <= pOnEdge2[0]; x++)
		{
			auto h = hListHorizontal[x - pOnEdge1[0]];
			auto reciprocalDepth = reciprocalDepthListHorizontal[x - pOnEdge1[0]];
			auto newColor = VectorHelper::ColorVectorScale(color, h);
			result.push_back(new Pixel(x, y, newColor, reciprocalDepth));
		}

	}


	return result;
}

std::vector<Pixel*> RasterizationRender::DrawShadedTextureTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2,
const double p0_depth, const double p1_depth, const double p2_depth, 
const double h0, const double h1, const double h2, 
 const std::array<int, 3>& color, const Texture* texture,
 const std::array<int, 2> uv0, const std::array<int, 2> uv1, const std::array<int, 2> uv2
 ) const
{
	std::vector<Pixel*> result;
	std::array<int, 2> p0_new = p0;
	std::array<int, 2> p1_new = p1;
	std::array<int, 2> p2_new = p2;
	double p0_depth_new = p0_depth;
	double p1_depth_new = p1_depth;
	double p2_depth_new = p2_depth;
	double h0_new = h0;
	double h1_new = h1;
	double h2_new = h2;
	std::array<int, 2> uv0_new = uv0;
	std::array<int, 2> uv1_new = uv1;
	std::array<int, 2> uv2_new = uv2;

	if (p0_new[1] > p1_new[1])
	{
		std::swap(p0_new, p1_new);
		std::swap(h0_new, h1_new);
		std::swap(p0_depth_new, p1_depth_new);
		std::swap(uv0_new, uv1_new);
	}
	if (p0_new[1] > p2_new[1])
	{
		std::swap(p0_new, p2_new);
		std::swap(h0_new, h2_new);
		std::swap(p0_depth_new, p2_depth_new);
		std::swap(uv0_new, uv2_new);
	}
	if (p1_new[1] > p2_new[1])
	{
		std::swap(p1_new, p2_new);
		std::swap(h1_new, h2_new);
		std::swap(p1_depth_new, p2_depth_new);
		std::swap(uv1_new, uv2_new);
	}
	auto xListLow = Interpolate(p0_new[1], p0_new[0], p2_new[1], p2_new[0]);
	auto hListLow = Interpolate(p0_new[1], h0_new, p2_new[1], h2_new);
	auto reciprocalDepthListLow = Interpolate(p0_new[1], 1/p0_depth_new, p2_new[1], 1/p2_depth_new);
	auto uListLow = Interpolate(p0_new[1], uv0_new[0]/p0_depth_new, p2_new[1], uv2_new[0]/p2_depth_new);
	auto vListLow = Interpolate(p0_new[1], uv0_new[1]/p0_depth_new, p2_new[1], uv2_new[1]/p2_depth_new);

	auto xListHigher = Interpolate(p1_new[1], p1_new[0], p2_new[1], p2_new[0]);
	auto hListHigher = Interpolate(p1_new[1], h1_new, p2_new[1], h2_new);
	auto reciprocalDepthListHigher = Interpolate(p1_new[1], 1/p1_depth_new, p2_new[1], 1/p2_depth_new);
	auto uListHigher = Interpolate(p1_new[1], uv1_new[0]/p1_depth_new, p2_new[1], uv2_new[0]/p2_depth_new);
	auto vListHigher = Interpolate(p1_new[1], uv1_new[1]/p1_depth_new, p2_new[1], uv2_new[1]/p2_depth_new);
	

	auto xListHighest = Interpolate(p0_new[1], p0_new[0], p1_new[1], p1_new[0]);
	auto hListHighest = Interpolate(p0_new[1], h0_new, p1_new[1], h1_new);
	auto reciprocalDepthListHighest = Interpolate(p0_new[1], 1/p0_depth_new, p1_new[1], 1/p1_depth_new);
	auto uListHighest = Interpolate(p0_new[1], uv0_new[0]/p0_depth_new, p1_new[1], uv1_new[0]/p1_depth_new);
	auto vListHighest = Interpolate(p0_new[1], uv0_new[1]/p0_depth_new, p1_new[1], uv1_new[1]/p1_depth_new);

	for (int y = p0_new[1]; y <= p2_new[1]; y++)
	{
		auto xOnEdge1 = xListLow[y - p0_new[1]];
		auto hOnEdge1 = hListLow[y - p0_new[1]];
		auto reciprocalDepthOnEdge1 = reciprocalDepthListLow[y - p0_new[1]];
		auto uOnEdge1 = uListLow[y - p0_new[1]];
		auto vOnEdge1 = vListLow[y - p0_new[1]];

		double hOnEdge2;
		double reciprocalDepthOnEdge2;
		double uOnEdge2;
		double vOnEdge2;

		std::array<int, 2> pOnEdge1 = { xOnEdge1,y };
		std::array<int, 2> pOnEdge2;
		if (y < p1_new[1])
		{
			hOnEdge2 = hListHighest[y - p0_new[1]];
			pOnEdge2 = { (int)xListHighest[y - p0_new[1]],y };
			reciprocalDepthOnEdge2 = reciprocalDepthListHighest[y - p0_new[1]];
			uOnEdge2 = uListHighest[y - p0_new[1]];
			vOnEdge2 = vListHighest[y - p0_new[1]];
		}
		else {
			hOnEdge2 = hListHigher[y - p1_new[1]];
			pOnEdge2 = { (int)xListHigher[y - p1_new[1]],y };
			reciprocalDepthOnEdge2 = reciprocalDepthListHigher[y - p1_new[1]];
			uOnEdge2 = uListHigher[y - p1_new[1]];
			vOnEdge2 = vListHigher[y - p1_new[1]];
		}
		if (pOnEdge1[0] > pOnEdge2[0])
		{
			std::swap(pOnEdge1, pOnEdge2);
			std::swap(hOnEdge1, hOnEdge2);
			std::swap(reciprocalDepthOnEdge1, reciprocalDepthOnEdge2);
			std::swap(uOnEdge1, uOnEdge2);
			std::swap(vOnEdge1, vOnEdge2);
		}
		auto hListHorizontal = Interpolate(pOnEdge1[0], hOnEdge1, pOnEdge2[0], hOnEdge2);
		auto reciprocalDepthListHorizontal = Interpolate(pOnEdge1[0], reciprocalDepthOnEdge1, pOnEdge2[0], reciprocalDepthOnEdge2);
		auto uListHorizontal = Interpolate(pOnEdge1[0], uOnEdge1, pOnEdge2[0], uOnEdge2);
		auto vListHorizontal = Interpolate(pOnEdge1[0], vOnEdge1, pOnEdge2[0], vOnEdge2);
		for (int x = pOnEdge1[0]; x <= pOnEdge2[0]; x++)
		{
			auto h = hListHorizontal[x - pOnEdge1[0]];
			auto reciprocalDepth = reciprocalDepthListHorizontal[x - pOnEdge1[0]];
			auto u = uListHorizontal[x - pOnEdge1[0]]/reciprocalDepth;
			auto v = vListHorizontal[x - pOnEdge1[0]]/reciprocalDepth;
			const auto textureColor = texture->GetPixel(u, v);

			auto newColor = VectorHelper::ColorVectorScale(textureColor, h);
			result.push_back(new Pixel(x, y, newColor, reciprocalDepth));
		}

	}


	return result;
}

std::array<double, 2> RasterizationRender::ProjectVertex(const std::array<double, 3>& vertex,const double d, const std::array<double, 3>& modelPosition) const
{
	const double dRate = d / (vertex[2] - camera->GetPosition()[2] + modelPosition[2]);
	const double x = (vertex[0] - camera->GetPosition()[0] + modelPosition[0]) * dRate;
	const double y = (vertex[1] - camera->GetPosition()[1] + modelPosition[1]) * dRate;

	return { x ,y };

}



void RasterizationRender::RunRender()
{
	canvas->resetCanvas();
	std::vector<const ModelInstance*> clippedInstances;
	std::vector<Pixel*> pixels;
	const auto matrix_camera = VectorHelper::MatrixMultiply(
		VectorHelper::Build4DInverseTranslationMatrix(camera->GetPosition()),
		VectorHelper::Build4DRotateInverseMatrix(camera->GetAngle(), camera->GetDirection())
	);
	lightList.reserve(originalLightList.size());
	for (const auto& light : originalLightList)
	{
		const auto newLight = new Light(light->GetLightType(),
		 VectorHelper::Build3DPoint(VectorHelper::VerticeMatrixMultiply(VectorHelper::BuildHomogeneousPoint(light->GetPosition()), matrix_camera)), 
		 VectorHelper::Build3DPoint(VectorHelper::VerticeMatrixMultiply(VectorHelper::BuildHomogeneousDirection(light->GetDirection()), matrix_camera)), 
		 light->GetIntensity());
		 
		lightList.push_back(newLight);
	}
	
	for (const auto& instance : instances)
	{
		const auto clippedInstance = ClipInstance(instance, matrix_camera);
		if (clippedInstance != nullptr)
		{
			clippedInstances.push_back(clippedInstance);
		}
	}
	
	// 使用 std::async 来并行处理 RenderInstance
	std::vector<std::future<std::vector<Pixel*>>> futures;

	for (const auto& instance : clippedInstances)
	{
		futures.emplace_back(std::async(std::launch::async, [this, &instance, &matrix_camera]() {
			return RenderInstance(*instance, matrix_camera);
		}));
	}

	// 收集结果
	for (auto& future : futures)
	{
		auto tmpPixels = future.get();
		pixels.insert(pixels.end(), tmpPixels.begin(), tmpPixels.end());
	}
	for (auto pixel : pixels)
	{
		const auto& position = pixel->GetPosition();
		const auto& color = pixel->GetColor();
		if (CompareAndSetDepthBuffer(position, pixel->GetReciprocalDepth()))
		{
			canvas->PutPixel( position[0], position[1], RGB(color[0], color[1], color[2]));
		}
		delete pixel;
	}
	for (const auto& instance : clippedInstances)
	{
		delete instance;
	}
	clippedInstances.clear();
	for (auto& light : lightList)
	{
		delete light;
	}
	lightList.clear();
}

std::vector<Pixel*> RasterizationRender::RenderInstance(const ModelInstance& instance, const std::array< std::array<double, 4>, 4>& matrix_camera) const
{
	std::vector<Pixel*> result;
	std::unordered_map<std::string, std::pair<std::array<int, 2>, double>> projectedPoints;
	auto matrix_viewport = VectorHelper::Build4DProjectionViewportToCanvasMatrix(canvas->getViewportDistance(), canvas->getCanvasWidth(), canvas->getCanvasHeight(), canvas->getViewportWidth(), canvas->getViewportHeight());



	for (auto& vertice : instance.GetVertices())
	{
		auto homoVertice = VectorHelper::BuildHomogeneousPoint(vertice);
		auto new2DVerticeWithDepth = VectorHelper::Build2DPointWithDepth(VectorHelper::VerticeMatrixMultiply(homoVertice, matrix_viewport));

		projectedPoints[Utils::ArrayToString(vertice)] = canvas->ConvertPointToCanvasCoordinateWithDepth(new2DVerticeWithDepth);
	}
	for (const auto& triangle : instance.GetTriangles())
	{
		const auto& p0 = projectedPoints[Utils::ArrayToString(triangle->GetV0())].first;
		const auto& p1 = projectedPoints[Utils::ArrayToString(triangle->GetV1())].first;
		const auto& p2 = projectedPoints[Utils::ArrayToString(triangle->GetV2())].first;
		const auto& depth0 = projectedPoints[Utils::ArrayToString(triangle->GetV0())].second;
		const auto& depth1 = projectedPoints[Utils::ArrayToString(triangle->GetV1())].second;
		const auto& depth2 = projectedPoints[Utils::ArrayToString(triangle->GetV2())].second;

		const auto h0 = triangle->GetH0();
		const auto h1 = triangle->GetH1();
		const auto h2 = triangle->GetH2();

		std::vector<Pixel*> pixels;
		if (triangle->GetTexture() != nullptr)
		{
			const auto uv0 = triangle->GetTextureUV0();
			const auto uv1 = triangle->GetTextureUV1();
			const auto uv2 = triangle->GetTextureUV2();
			pixels = DrawShadedTextureTriangle(p0, p1, p2, depth0, depth1, depth2, h0, h1, h2, triangle->GetColor(), triangle->GetTexture(), uv0, uv1, uv2);
		}
		else
		{
			pixels = DrawShadedTriangle(p0, p1, p2, depth0, depth1, depth2, h0, h1, h2, triangle->GetColor());
		}
		for (auto pixel : pixels)
		{
			if (pixel->GetPosition()[0] >= 0 && pixel->GetPosition()[0] <= canvas->getCanvasWidth() && pixel->GetPosition()[1] >= 0 && pixel->GetPosition()[1] <= canvas->getCanvasHeight())
			{
				result.push_back(pixel);
			}
			else
			{
				delete pixel;
			}
		}

	}
	return result;
}

void RasterizationRender::SetCanvas(Canvas* canvasPointer)
{
	canvas = canvasPointer;
	InitDepthBuffer();
}

ModelInstance* RasterizationRender::ClipInstance( ModelInstance* instance, const std::array< std::array<double, 4>, 4>& matrix_camera)
{
	const Model* model = instance->GetModel();
	auto matrix_model_scale = VectorHelper::Build4DScaleMatrix(instance->GetScale());
	auto matrix_model_rotate = VectorHelper::Build4DRotationMatrix(instance->GetAngle(), instance->GetRotate());
	auto matrix_model_translate = VectorHelper::Build4DTranslationMatrix(instance->GetPosition());
	auto matrix_model = VectorHelper::MatrixMultiply(VectorHelper::MatrixMultiply(matrix_model_scale, matrix_model_rotate), matrix_model_translate);
	auto matrix_viewport = VectorHelper::Build4DProjectionViewportToCanvasMatrix(canvas->getViewportDistance(), canvas->getCanvasWidth(), canvas->getCanvasHeight(), canvas->getViewportWidth(), canvas->getViewportHeight());	
	auto matrix_model_camera = VectorHelper::MatrixMultiply(matrix_model, matrix_camera);

	ModelInstance* newInstance = CreateNewInstance(instance, matrix_model_camera);


	ModelInstance* clippedInstance = nullptr;
	for (const auto& plane : planes)
	{
		clippedInstance = ClipInstanceAgainstPlane(newInstance, plane, matrix_model_camera);
		if (clippedInstance==nullptr)
		{
			return nullptr;
		}
	}
	return newInstance;

}

ModelInstance* RasterizationRender::ClipInstanceAgainstPlane( ModelInstance* instance,  const Plane* plane, const std::array< std::array<double, 4>, 4>& matrix_model_camera)
{
 
	const auto instanceBSCenterPoint = instance->GetBoundingSphere()->GetCenterPoint();
	const auto instanceBSRadius = instance->GetBoundingSphere()->GetRadius();
	const double d = VectorHelper::GetSingnedVertexToPlaneDistance(instanceBSCenterPoint, plane);

	if (d > instanceBSRadius)
	{
		return instance;

	} else if( d < -instanceBSRadius)
	{
		
		return nullptr;
	}
	else
	{
		auto clippedTriangles = ClipTrianglesAgainstPlane(instance, plane, matrix_model_camera);
		instance->SetTriangles(clippedTriangles);
		for (auto triangle : clippedTriangles)
		{
			delete triangle;
		}
		return instance;
	}

}

bool RasterizationRender::IsBackTriangle(const Triangle* triangle, const std::array< std::array<double, 4>, 4>& matrix_model_camera)
{	
	const auto normal = triangle->GetNormal();
	const auto cameraPosition = camera->GetPosition();
	const auto centroid = VectorHelper::GetCentroid(triangle->GetV0(), triangle->GetV1(), triangle->GetV2());
	const auto cameraDirectionVector = VectorHelper::VectorSub({0,0,0}, centroid);
	const auto cos = VectorHelper::GetCosBetweenVectors(normal, cameraDirectionVector);
	if (cos > 0)
	{
		return false;
	}
	return true;

}

std::vector<Triangle*> RasterizationRender::ClipTrianglesAgainstPlane(const ModelInstance* instance, const Plane* plane, const std::array< std::array<double, 4>, 4>& matrix_model_camera)
{
	const auto triangles = instance->GetTriangles();
	std::vector<Triangle*> clippedTriangles;
	for (const auto& triangle : triangles)
	{
		const auto clippedTrianglesResult = ClipTriangleAgainstPlane(triangle, plane, matrix_model_camera);
		for (auto clippedTriangle : clippedTrianglesResult)
		{
			clippedTriangles.push_back(clippedTriangle);
		}
	}
	return clippedTriangles;
}

std::vector<Triangle*>  RasterizationRender::ClipTriangleAgainstPlane(const Triangle* triangle, const Plane* plane, const std::array< std::array<double, 4>, 4>& matrix_model_camera)
{
	const double d0 = VectorHelper::GetSingnedVertexToPlaneDistance(triangle->GetV0(), plane);
	const double d1 = VectorHelper::GetSingnedVertexToPlaneDistance(triangle->GetV1(), plane);
	const double d2 = VectorHelper::GetSingnedVertexToPlaneDistance(triangle->GetV2(), plane);
	const auto texture = triangle->GetTexture();
	const auto uv0 = triangle->GetTextureUV0();
	const auto uv1 = triangle->GetTextureUV1();
	const auto uv2 = triangle->GetTextureUV2();
	if (d0 >=0 && d1 >= 0 && d2 >= 0)
	{
		return { new Triangle(*triangle) };
	} 
	else if (d0 < 0 && d1 < 0 && d2 < 0)
	{
		return {};
	}

	else if (d0 > 0 && d1 < 0 && d2 < 0)
	{
		auto pointNewBAndT = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV0(), triangle->GetV1());
		auto pointNewCAndT = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV0(), triangle->GetV2());
		auto h0 = triangle->GetH0();
		auto newHB = h0 + pointNewBAndT.second*(triangle->GetH1() - h0);
		auto newHC = h0 + pointNewCAndT.second*(triangle->GetH2() - h0);

		return { new Triangle(triangle->GetV0(), pointNewBAndT.first, pointNewCAndT.first, triangle->GetColor(), texture, uv0, uv1, uv2, h0, newHB, newHC)};
	} else if (d0 < 0 && d1 > 0 && d2 < 0){
		auto pointNewAAndT = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV1(), triangle->GetV2());
		auto pointNewCAndT = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV1(), triangle->GetV0());
		auto h1 = triangle->GetH1();
		auto newHA = h1 + pointNewAAndT.second*(triangle->GetH2() - h1);
		auto newHC = h1 + pointNewCAndT.second*(triangle->GetH0() - h1);
		
		return { new Triangle(pointNewAAndT.first, triangle->GetV1(), pointNewCAndT.first, triangle->GetColor(), texture, uv0, uv1, uv2, newHA, h1, newHC)};
	} else if (d0 < 0 && d1 < 0 && d2 > 0){
		auto pointNewA = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV2(), triangle->GetV0());
		auto pointNewB = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV2(), triangle->GetV1());
		auto h2 = triangle->GetH2();
		auto newHA = h2 + pointNewA.second*(triangle->GetH0() - h2);
		auto newHB = h2 + pointNewB.second*(triangle->GetH1() - h2);
		return { new Triangle(pointNewA.first, pointNewB.first, triangle->GetV2(), triangle->GetColor(), texture, uv0, uv1, uv2, newHA, newHB, h2)};
	} else if (d0 < 0 && d1 > 0 && d2 > 0){
		auto pointNewB = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV1(), triangle->GetV0());
		auto pointNewC = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV2(), triangle->GetV0());
		auto h1 = triangle->GetH1();
		auto h2 = triangle->GetH2();
		auto newHB = h1 + pointNewB.second*(triangle->GetH0() - h1);
		auto newHC = h2 + pointNewC.second*(triangle->GetH0() - h2);
		return { new Triangle(triangle->GetV1(), pointNewB.first, pointNewC.first, triangle->GetColor(), texture, uv0, uv1, uv2, h1, newHB, newHC), new Triangle(triangle->GetV1(), triangle->GetV2(), pointNewC.first, triangle->GetColor(), texture, uv0, uv1, uv2, h1, h2, newHC)};
	} else if (d0 > 0 && d1 < 0 && d2 > 0){
		auto pointNewA = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV0(), triangle->GetV1());
		auto pointNewC = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV2(), triangle->GetV1());
		auto h0 = triangle->GetH0();
		auto h2 = triangle->GetH2();
		auto newHA = h0 + pointNewA.second*(triangle->GetH1() - h0);
		auto newHC = h2 + pointNewC.second*(triangle->GetH1() - h2);
		return { new Triangle(triangle->GetV0(), pointNewA.first, pointNewC.first, triangle->GetColor(), texture, uv0, uv1, uv2, h0, newHA, newHC), new Triangle(triangle->GetV2(), triangle->GetV0(), pointNewC.first, triangle->GetColor(), texture, uv0, uv1, uv2, h2, h0, newHC)};
	} else if (d0 > 0 && d1 > 0 && d2 < 0){
		auto pointNewA = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV0(), triangle->GetV2());
		auto pointNewB = VectorHelper::GetPlaneLineIntersection(plane, triangle->GetV1(), triangle->GetV2());
		auto h0 = triangle->GetH0();
		auto h1 = triangle->GetH1();
		auto newHA = h0 + pointNewA.second*(triangle->GetH1() - h0);
		auto newHB = h1 + pointNewB.second*(triangle->GetH1() - h1);
		return { new Triangle(triangle->GetV0(), pointNewA.first, pointNewB.first, triangle->GetColor(), texture, uv0, uv1, uv2, h0, newHA, newHB), new Triangle(triangle->GetV1(), triangle->GetV0(), pointNewB.first, triangle->GetColor(), texture, uv0, uv1, uv2, h1, h0, newHB)};
	}
	throw std::runtime_error("Triangle can not be clipped.");
}


ModelInstance* RasterizationRender::CreateNewInstance(const ModelInstance* instance, const std::array< std::array<double, 4>, 4>& matrix_model_camera)
{
	const Model* model = instance->GetModel();
	auto modelBoundingSphere = model->GetBoundingSphere();
	if (modelBoundingSphere==nullptr)
	{
		throw std::runtime_error("Model bounding sphere is null.");
	}


	const auto modelBoundingSphereCenterPoint = modelBoundingSphere->GetCenterPoint();
	const auto modelRadius = modelBoundingSphere->GetRadius();
    const auto homoCenterPoint = VectorHelper::BuildHomogeneousPoint(modelBoundingSphereCenterPoint);
	const auto instanceBSCenterPoint = VectorHelper::Build3DPoint(VectorHelper::VerticeMatrixMultiply(homoCenterPoint, matrix_model_camera)) ;
	auto instanceBSRadius = modelRadius * instance->GetScale();
	ModelInstance* newInstance = new ModelInstance(*instance);
	BoundingSphere* newBoundingSphere = new BoundingSphere(instanceBSCenterPoint, instanceBSRadius);
	newInstance->SetBoundingSphere(newBoundingSphere);	
	delete newBoundingSphere;
	const auto triangles = newInstance->GetTriangles();
	const auto vertices = instance->GetVertices();
	std::unordered_map<std::string, std::array<double, 3>> calculatedPoints;
	for (auto& vertex : vertices){
		const auto homoVertex = VectorHelper::BuildHomogeneousPoint(vertex);
		const auto newVertex = VectorHelper::Build3DPoint(VectorHelper::VerticeMatrixMultiply(homoVertex, matrix_model_camera));
		calculatedPoints[Utils::ArrayToString(vertex)] = newVertex;
	}
	std::vector<Triangle*> newTriangles;
	for (auto& triangle : triangles){

		const auto v0 = calculatedPoints[Utils::ArrayToString(triangle->GetV0())];
		const auto v1 = calculatedPoints[Utils::ArrayToString(triangle->GetV1())];
		const auto v2 = calculatedPoints[Utils::ArrayToString(triangle->GetV2())];

		const auto normal0 = VectorHelper::Build3DPoint(VectorHelper::VerticeMatrixMultiply(VectorHelper::BuildHomogeneousDirection(triangle->GetNormal0()), matrix_model_camera));
		const auto normal1 = VectorHelper::Build3DPoint(VectorHelper::VerticeMatrixMultiply(VectorHelper::BuildHomogeneousDirection(triangle->GetNormal1()), matrix_model_camera));
		const auto normal2 = VectorHelper::Build3DPoint(VectorHelper::VerticeMatrixMultiply(VectorHelper::BuildHomogeneousDirection(triangle->GetNormal2()), matrix_model_camera));
		const auto texture = triangle->GetTexture();
		const auto uv0 = triangle->GetTextureUV0();
		const auto uv1 = triangle->GetTextureUV1();
		const auto uv2 = triangle->GetTextureUV2();
		const auto newTriangle = new Triangle(v0, v1, v2, normal0, normal1, normal2, triangle->GetColor(), texture, uv0, uv1, uv2, triangle->GetH0(), triangle->GetH1(), triangle->GetH2());
		if (IsBackTriangle(newTriangle, matrix_model_camera))
		{
			delete newTriangle;
			continue;
		}	
		//Gouraud Shading
		const auto cameraPosition = camera->GetPosition();


		////Computed triangle normals
		//const auto h0 = GetLighteningScale(newTriangle->GetV0(), newTriangle->GetNormal(), 50);
		//const auto h1 = GetLighteningScale(newTriangle->GetV1(), newTriangle->GetNormal(), 50);
		//const auto h2 = GetLighteningScale(newTriangle->GetV2(), newTriangle->GetNormal(), 50);

		
		 //Vertex normals from model
		const auto h0 = GetLighteningScale(newTriangle->GetV0(), normal0, 50);
		const auto h1 = GetLighteningScale(newTriangle->GetV1(), normal1, 50);
		const auto h2 = GetLighteningScale(newTriangle->GetV2(), normal2, 50);
		newTriangle->SetH0(h0);
		newTriangle->SetH1(h1);
		newTriangle->SetH2(h2);

		
		newTriangles.push_back(newTriangle);
	}
	newInstance->SetTriangles(newTriangles);
	for (auto& triangle : newTriangles){
		delete triangle;
	}
	return newInstance;
}


bool RasterizationRender::CompareAndSetDepthBuffer(const std::array<int, 2>& point, const double reciprocalDepth)
{
	if (depthBuffer[point[0]][point[1]] < reciprocalDepth)
	{
		depthBuffer[point[0]][point[1]] = reciprocalDepth;
		return true;
	}
	return false;
}

const std::vector<Light*> RasterizationRender::GetLightList() const
{
	return lightList;
}

