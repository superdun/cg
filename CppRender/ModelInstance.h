#pragma once
#include "Model.h"
#include "Transform.h"
class ModelInstance
{
private:
	const Model* model;
	std::array<double, 3> position;
	const Transform* transform;
	std::vector<Triangle*> triangles;
public:
	ModelInstance(const Model* model, const std::array<double, 3>& position,const Transform* transform);
	ModelInstance(const ModelInstance& other);
	~ModelInstance();
	const Model* GetModel() const;
	const std::array<double, 3>& GetPosition() const;
	const std::string& GetModelName() const;
	const Transform* GetTransform() const;
	const double& GetScale() const;
	const std::array<double, 3>& GetRotate() const;
	const std::array<double, 3>& GetTranslation() const;
	const double& GetAngle() const;
	const std::vector<Triangle*>& GetTriangles() const;
	const std::vector<std::array<double, 3>> GetVertices() const;
	void SetTriangles(const std::vector<Triangle*>& triangles);
};