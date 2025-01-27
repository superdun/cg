#include "Model.h"
#include <string>
#include <algorithm>
#include <unordered_map>
#include "Utils.h"

Model::Model(const std::string name,const std::vector<Triangle*>& triangles, const BoundingSphere* boundingSphere)
    : name(name), triangles(triangles),boundingSphere(boundingSphere)
{
}

Model::~Model()
{

}

const std::vector< std::array<double, 3>> Model::GetVertices() const
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
