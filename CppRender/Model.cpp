#include "Model.h"
#include <string>
#include <algorithm>

Model::Model(const std::string name,const std::vector<Triangle*>& triangles, const BoundingSphere* boundingSphere)
    : name(name), triangles(triangles),boundingSphere(boundingSphere)
{
}

Model::~Model()
{

}

const std::vector<std::array<double, 3>> Model::GetVertices() const
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


const std::vector<Triangle*> Model::GetTriangles() const
{
    return triangles;
}

const std::string Model::GetName() const
{
    return name;
}

const BoundingSphere* Model::GetBoundingSphere() const
{
    return boundingSphere;
}
