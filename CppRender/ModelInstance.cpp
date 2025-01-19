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
