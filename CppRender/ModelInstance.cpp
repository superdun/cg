#include "ModelInstance.h"

ModelInstance::ModelInstance(const Model* model, const std::array<double, 3>& position, const Transform* transform)
	: model(model), position(position), transform(transform)
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
	return model->getName();
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
