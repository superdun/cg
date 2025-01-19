#include "Model.h"
#include <string>

Model::Model(const std::string name,const std::vector<std::array<double, 3>>& vertices, const std::vector<Triangle>& triangles)
    : name(name), vertices(vertices), triangles(triangles)
{
}

Model::~Model()
{

}

const std::vector<std::array<double, 3>>& Model::getVertices() const
{
    return vertices;
}


const std::vector<Triangle>& Model::getTriangles() const
{
    return triangles;
}

const std::string Model::getName() const
{
    return name;
}
