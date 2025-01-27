#include "ModelInstance.h"
#include <unordered_map>
#include "Utils.h"

ModelInstance::ModelInstance(const Model* model, const std::array<double, 3>& position, const Transform* transform)
	: model(model), position(position), transform(transform)

{	
	const auto triangles = model->GetTriangles();
	for (const auto& triangle : triangles)
	{
		this->triangles.push_back(new Triangle(*triangle));
	}
	boundingSphere = new BoundingSphere(*model->GetBoundingSphere());
}

ModelInstance::ModelInstance(const ModelInstance& other)
	: model(other.model), position(other.position), transform(other.transform)
{
	const auto triangles = other.GetModel()->GetTriangles();
	for (const auto& triangle : triangles)
	{
		this->triangles.push_back(new Triangle(*triangle));
	}
	boundingSphere = new BoundingSphere(*other.boundingSphere);
}

ModelInstance::~ModelInstance()
{
	for (const auto& triangle : triangles)
	{
		delete triangle;
	}
	delete boundingSphere;
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

const std::vector< std::array<double, 3>> ModelInstance::GetVertices() const
{
	std::unordered_map<std::string, std::array<double, 3>> map;

	for (const auto& triangle : triangles)
	{
		const auto v0 = triangle->GetV0();
		const auto v1 = triangle->GetV1();
		const auto v2 = triangle->GetV2();

		map[Utils::ArrayToString(v0)] = v0;
		map[Utils::ArrayToString(v1)] = v1;
		map[Utils::ArrayToString(v2)] = v2;
	}


	std::vector<std::array<double, 3>> uniquePoints;
	uniquePoints.reserve(map.size());

	for (const auto& pair : map)
	{
		uniquePoints.push_back(pair.second);
	}

	return uniquePoints;
}

void ModelInstance::SetTriangles(const std::vector<Triangle*>& triangles)
{
 for (auto& triangle : this->triangles) {
        delete triangle;
    }
    this->triangles.clear();


    this->triangles.reserve(triangles.size());
    for (const auto& triangle : triangles) {
        this->triangles.push_back(new Triangle(*triangle)); 
    }
}

void ModelInstance::SetBoundingSphere(const BoundingSphere* boundingSphere)
{
	if (this->boundingSphere != nullptr)
	{
		delete this->boundingSphere;
	}
	this->boundingSphere = new BoundingSphere(*boundingSphere);
}

const BoundingSphere* ModelInstance::GetBoundingSphere() const
{
	return boundingSphere;
}

