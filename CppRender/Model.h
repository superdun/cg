#pragma once
#include <array>
#include <vector>
#include "Triangle.h"
#include <string>
#include "BoundingSphere.h"
class Model
{
private:
    const std::vector<Triangle*> triangles;
	const std::string name;
    const BoundingSphere* boundingSphere;
public:
    Model(const std::string name, const std::vector<Triangle*>& triangles, const BoundingSphere* boundingSphere);
    ~Model();
    const std::vector<std::array<double, 3>> GetVertices() const;

    const std::vector<Triangle*> GetTriangles() const;
	const std::string GetName() const;
    const BoundingSphere* GetBoundingSphere() const;
};

