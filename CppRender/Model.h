#pragma once
#include <array>
#include <vector>
#include "Triangle.h"
#include <string>
class Model
{
private:
    const std::vector<std::array<double, 3>>& vertices;
    const std::vector<Triangle>& triangles;
	const std::string name;
public:
    Model(const std::string name,const std::vector<std::array<double, 3>>& vertices, const std::vector<Triangle>& triangles);
    ~Model();
    const std::vector<std::array<double, 3>>& getVertices() const;

    const std::vector<Triangle>& getTriangles() const;
	const std::string getName() const;
};

