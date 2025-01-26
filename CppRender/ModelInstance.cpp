#include "ModelInstance.h"

ModelInstance::ModelInstance(const Model* model, const std::array<double, 3>& position, const Transform* transform)
	: model(model), position(position), transform(transform)

{	
	triangles = model->GetTriangles();
}

ModelInstance::ModelInstance(const ModelInstance& other)
	: model(other.model), position(other.position), transform(other.transform),
	  triangles(other.triangles)
{

}

ModelInstance::~ModelInstance()
{

}

const Model* ModelInstance::GetModel() const
{
    return model;
}

const std::array<double, 3>& ModelInstance::GetPosition() const
{
	return position;
}

const std::string& ModelInstance::GetModelName() const
{
	return model->GetName();
}

const Transform* ModelInstance::GetTransform() const
{
	return transform;
}

const double& ModelInstance::GetScale() const
{
	if (transform != nullptr)
	{
		return transform->GetScale();
	}
	throw "Transform is null";
}

const std::array<double, 3>& ModelInstance::GetRotate() const
{
	if (transform!=nullptr)
	{
		return transform->GetRotate();
	}
	throw "Transform is null";
}

const std::array<double, 3>& ModelInstance::GetTranslation() const
{
	if (transform != nullptr)
	{
		return transform->GetTranslation();
	}
	throw "Transform is null";
}

const double& ModelInstance::GetAngle() const
{
	if (transform != nullptr)
	{
		return transform->GetAngle();
	}
	throw "Transform is null";
}

const std::vector<Triangle*>& ModelInstance::GetTriangles() const
{
	return triangles;
}

const std::vector<std::array<double, 3>> ModelInstance::GetVertices() const
{
	std::vector<std::array<double, 3>> result;
	for (auto& triangle : triangles)
	{
		if (std::find(result.begin(), result.end(), triangle->GetV0()) == result.end())
		{
			result.push_back(triangle->GetV0());
		}
		if (std::find(result.begin(), result.end(), triangle->GetV1()) == result.end())
		{
			result.push_back(triangle->GetV1());
		}
		if (std::find(result.begin(), result.end(), triangle->GetV2()) == result.end())
		{
			result.push_back(triangle->GetV2());
		}
	}

	return result;
}

void ModelInstance::SetTriangles(const std::vector<Triangle*>& triangles)
{
	this->triangles = triangles;
}

