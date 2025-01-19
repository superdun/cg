#pragma once
#include "Model.h"
#include "Transform.h"
class ModelInstance
{
private:
	const Model* model;
	std::array<double, 3> position;
	const Transform* transform;
public:
	ModelInstance(const Model* model, const std::array<double, 3>& position,const Transform* transform);
	~ModelInstance();
	const Model* GetModel() const;
	const std::array<double, 3>& GetPosition() const;
	const std::string& GetModelName() const;
};