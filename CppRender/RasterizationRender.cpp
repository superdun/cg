#include "RasterizationRender.h"
#include <algorithm>
#include "VectorHelper.h"
#include <unordered_map>
#include "Utils.h"


RasterizationRender::RasterizationRender(Canvas *canvas, const std::vector<ModelInstance *> &instances, const Camera *camera, const std::array<Plane *, 5> &planes)
    : instances(instances), canvas(canvas), camera(camera), planes(planes)
{
}


RasterizationRender::~RasterizationRender()
{
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
 
std::vector<Pixel*> RasterizationRender::DrawLine(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 3> color) const
{
	std::array<int, 2> p0_new = p0;
	std::array<int, 2> p1_new = p1;
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
			result.push_back(new Pixel(xList[p - p0_new[1]], p, color));
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
			result.push_back(new Pixel(p, yList[p - p0_new[0]], color));
		}
	}

	return result;
}

std::vector<Pixel*> RasterizationRender::DrawWireFrameTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const std::array<int, 3>& color) const
{


	std::vector<Pixel*> result;
	auto line1 = DrawLine(p0, p1, color);
	auto line2 = DrawLine(p1, p2, color);
	auto line3 = DrawLine(p0, p2, color);
	// Insert the resulting pixels from each line into the result vector
	result.insert(result.end(), line1.begin(), line1.end());
	result.insert(result.end(), line2.begin(), line2.end());
	result.insert(result.end(), line3.begin(), line3.end());
	return result;
}

std::vector<Pixel*> RasterizationRender::DrawFilledTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2, const std::array<int, 3>& color) const
{
	std::vector<Pixel*> result;
	std::array<int, 2> p0_new = p0;
	std::array<int, 2> p1_new = p1;
	std::array<int, 2> p2_new = p2;
	if (p0_new[1]>p1_new[1])
	{
		std::swap(p0_new, p1_new);
	}
	if (p0_new[1] > p2_new[1])
	{
		std::swap(p0_new, p2_new);
	}
	if (p1_new[1] > p2_new[1])
	{
		std::swap(p1_new, p2_new);
	}
	auto xListLow = Interpolate(p0_new[1], p0_new[0], p2_new[1], p2_new[0]);
	auto xListHigher = Interpolate(p1_new[1], p1_new[0], p2_new[1], p2_new[0]);
	auto xListHighest = Interpolate(p0_new[1], p0_new[0], p1_new[1], p1_new[0]);
	
	
	for (int y = p0_new[1]; y <= p2_new[1]; y++)
	{
		auto xLow = xListLow[y - p0_new[1]];
		std::array<int, 2> pOnEdge1 = { xLow,y };
		std::array<int, 2> pOnEdge2;
		if (y< p1_new[1])
		{	
			
			pOnEdge2 = { (int)xListHighest[y- p0_new[1]],y};

		}
		else {
			pOnEdge2 = { (int)xListHigher[y - p1_new[1]],y };
		}
		if (pOnEdge1[0]> pOnEdge2[0])
		{
			std::swap(pOnEdge1, pOnEdge2);
		}
		for (int x = pOnEdge1[0]; x <= pOnEdge2[0]; x++)
		{
			result.push_back(new Pixel(x,y,color));
		}

	}


	return result;
}

std::vector<Pixel*> RasterizationRender::DrawShadedTriangle(const std::array<int, 2> p0, const std::array<int, 2> p1, const std::array<int, 2> p2,const double h0, const double h1, const double h2,  const std::array<int, 3>& color) const
{
	std::vector<Pixel*> result;
	std::array<int, 2> p0_new = p0;
	std::array<int, 2> p1_new = p1;
	std::array<int, 2> p2_new = p2;
	double h0_new = h0;
	double h1_new = h1;
	double h2_new = h2;
	if (p0_new[1] > p1_new[1])
	{
		std::swap(p0_new, p1_new);
		std::swap(h0_new, h1_new);
	}
	if (p0_new[1] > p2_new[1])
	{
		std::swap(p0_new, p2_new);
		std::swap(h0_new, h2_new);
	}
	if (p1_new[1] > p2_new[1])
	{
		std::swap(p1_new, p2_new);
		std::swap(h1_new, h2_new);
	}
	auto xListLow = Interpolate(p0_new[1], p0_new[0], p2_new[1], p2_new[0]);
	auto hListLow = Interpolate(p0_new[1], h0_new, p2_new[1], h2_new);

	auto xListHigher = Interpolate(p1_new[1], p1_new[0], p2_new[1], p2_new[0]);
	auto hListHigher = Interpolate(p1_new[1], h1_new, p2_new[1], h2_new);

	auto xListHighest = Interpolate(p0_new[1], p0_new[0], p1_new[1], p1_new[0]);
	auto hListHighest = Interpolate(p0_new[1], h0_new, p1_new[1], h1_new);

	for (int y = p0_new[1]; y <= p2_new[1]; y++)
	{
		auto xOnEdge1 = xListLow[y - p0_new[1]];
		auto hOnEdge1 = hListLow[y - p0_new[1]];
		double  hOnEdge2;
		std::array<int, 2> pOnEdge1 = { xOnEdge1,y };
		std::array<int, 2> pOnEdge2;
		if (y < p1_new[1])
		{
			hOnEdge2 = hListHighest[y - p0_new[1]];
			pOnEdge2 = { (int)xListHighest[y - p0_new[1]],y };

		}
		else {
			hOnEdge2 = hListHigher[y - p1_new[1]];
			pOnEdge2 = { (int)xListHigher[y - p1_new[1]],y };
		}
		if (pOnEdge1[0] > pOnEdge2[0])
		{
			std::swap(pOnEdge1, pOnEdge2);
			std::swap(hOnEdge1, hOnEdge2);
		}
		auto hListHorizontal = Interpolate(pOnEdge1[0], hOnEdge1, pOnEdge2[0], hOnEdge2);
		for (int x = pOnEdge1[0]; x <= pOnEdge2[0]; x++)
		{
			auto h = hListHorizontal[x - pOnEdge1[0]];
			auto newColor = VectorHelper::ColorVectorScale(color, h);
			result.push_back(new Pixel(x, y, newColor));
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

std::vector<Pixel*> RasterizationRender::RenderInstance(const ModelInstance& instance, const std::array< std::array<double, 4>, 4>& matrix_camera) const
{
	std::vector<Pixel*> result;
	std::unordered_map<std::string, std::array<int, 2>> projectedPoints;
	const Model* model = instance.GetModel();
	auto matrix_model_scale = VectorHelper::Build4DScaleMatrix(instance.GetScale());
	auto matrix_model_rotate = VectorHelper::Build4DRotationMatrix(instance.GetAngle(), instance.GetRotate());
	auto matrix_model_translate = VectorHelper::Build4DTranslationMatrix(instance.GetPosition());
	auto matrix_model = VectorHelper::MatrixMultiply(VectorHelper::MatrixMultiply(matrix_model_scale, matrix_model_rotate), matrix_model_translate);
	auto matrix_viewport = VectorHelper::Build4DProjectionViewportToCanvasMatrix(canvas->getViewportDistance(), canvas->getCanvasWidth(), canvas->getCanvasHeight(), canvas->getViewportWidth(), canvas->getViewportHeight());	
	auto matrix_model_camera = VectorHelper::MatrixMultiply(matrix_model, matrix_camera);
	


	for (auto& vertice : instance.GetVertices())
	{
		auto homoVertice = VectorHelper::BuildHomogeneousPoint(vertice);
		auto m = VectorHelper::MatrixMultiply(matrix_model_camera,matrix_viewport);
		auto newVertice = VectorHelper::Build2DPoint(VectorHelper::VerticeMatrixMultiply(homoVertice,m));

		projectedPoints[Utils::ArrayToString(vertice)] = canvas->ConvertPointToCanvasCoordinate(newVertice);
	}
	for (const auto& triangle : instance.GetTriangles())
	{
		const auto& p0 = projectedPoints[Utils::ArrayToString(triangle->GetV0())];
		const auto& p1 = projectedPoints[Utils::ArrayToString(triangle->GetV1())];
		const auto& p2 = projectedPoints[Utils::ArrayToString(triangle->GetV2())];

		auto pixels = DrawWireFrameTriangle(p0, p1, p2, triangle->GetColor());
		result.insert(result.end(), pixels.begin(), pixels.end());
	}
	return result;
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
	for (const auto& instance : instances)
	{
		const auto clippedInstance = ClipInstance(instance, matrix_camera);
		if (clippedInstance != nullptr)
		{
			clippedInstances.push_back(clippedInstance);
		}
	}
	
	for (const auto& instance : clippedInstances)
	{
		auto tmpPixels = RenderInstance(*instance, matrix_camera);
		pixels.insert(pixels.end(), tmpPixels.begin(), tmpPixels.end());

	}
	for (auto pixel : pixels)
	{
		const auto& position = pixel->GetPosition();
		const auto& color = pixel->GetColor();
		canvas->PutPixel( position[0], position[1], RGB(color[0], color[1], color[2]));
		delete pixel;
	}
	for (const auto& instance : clippedInstances)
	{
		delete instance;
	}
	clippedInstances.clear();
}

void RasterizationRender::ClipPipeline()
{
}

ModelInstance* RasterizationRender::ClipInstance(const ModelInstance* instance, const std::array< std::array<double, 4>, 4>& matrix_model_camera)
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
	ModelInstance* clippedInstance = nullptr;
	bool inScopeOfPlanes = false;
	for (const auto& plane : planes)
	{
		delete clippedInstance;
		clippedInstance = ClipInstanceAgainstPlane(instance, instanceBSCenterPoint, instanceBSRadius, plane, matrix_model_camera);
		if (clippedInstance==nullptr)
		{
			return nullptr;
		}
	}
	return clippedInstance;

}

ModelInstance* RasterizationRender::ClipInstanceAgainstPlane(const ModelInstance* instance, const std::array<double, 3>& instanceBSCenterPoint, const double instanceBSRadius, const Plane* plane, const std::array< std::array<double, 4>, 4>& matrix_model_camera)
{
	const double d = VectorHelper::GetSingnedVertexToPlaneDistance(instanceBSCenterPoint, plane);
	ModelInstance* newInstance = new ModelInstance(*instance);
	if (d > instanceBSRadius)
	{
		return newInstance;

	} else if( d < -instanceBSRadius)
	{
		return nullptr;
	}
	else
	{
		return newInstance;
	}

}

ModelInstance* RasterizationRender::ClipTrianglesAgainstPlane(const ModelInstance* instance, const std::array< std::array<double, 4>, 4>& matrix_model_camera)
{
	return nullptr;
}

Triangle* RasterizationRender::ClipTriangleAgainstPlane(const Triangle* triangle, const std::array< std::array<double, 4>, 4>& matrix_model_camera)
{
	return nullptr;
}

void RasterizationRender::SetCanvas( Canvas* canvasPointer)
{
	canvas = canvasPointer;
}





