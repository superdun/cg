#include "Triangle.h"
#include "VectorHelper.h"

Triangle::Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2, const std::array<double, 3> normal0, const std::array<double, 3> normal1, const std::array<double, 3> normal2, const std::array<int, 3> color)
    : v0(v0), v1(v1), v2(v2), color(color), h0(1), h1(1), h2(1), normal0(normal0), normal1(normal1), normal2(normal2), normal(VectorHelper::GetNormal(v0, v1, v2)), texture(nullptr), textureUV0({0, 0}), textureUV1({0, 0}), textureUV2({0, 0})
{

}

Triangle::Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2, const std::array<int, 3> color, const Texture* texture, const std::array<int, 2> textureUV0, const std::array<int, 2> textureUV1, const std::array<int, 2> textureUV2)
    : v0(v0), v1(v1), v2(v2), color(color), texture(texture), textureUV0(textureUV0), textureUV1(textureUV1), textureUV2(textureUV2), h0(1), h1(1), h2(1), normal(VectorHelper::GetNormal(v0, v1, v2))
{
    normal0 = normal;
    normal1 = normal;
    normal2 = normal;
}
Triangle::Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2, const std::array<int, 3> color, const Texture* texture, const std::array<int, 2> textureUV0, const std::array<int, 2> textureUV1, const std::array<int, 2> textureUV2, const double h0, const double h1, const double h2)
    : v0(v0), v1(v1), v2(v2), color(color), texture(texture), textureUV0(textureUV0), textureUV1(textureUV1), textureUV2(textureUV2), h0(h0), h1(h1), h2(h2), normal(VectorHelper::GetNormal(v0, v1, v2))
{
    normal0 = normal;
    normal1 = normal;
    normal2 = normal;
}

Triangle::Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2,
                  const std::array<double, 3> normal0, const std::array<double, 3> normal1, const std::array<double, 3> normal2, const std::array<int, 3> color, const Texture* texture, const std::array<int, 2> textureUV0, const std::array<int, 2> textureUV1, const std::array<int, 2> textureUV2)
    : v0(v0), v1(v1), v2(v2), h0(1), h1(1), h2(1), normal0(normal0), normal1(normal1), normal2(normal2), normal(VectorHelper::GetNormal(v0, v1, v2)), color(color), texture(texture), textureUV0(textureUV0), textureUV1(textureUV1), textureUV2(textureUV2)
{
}
Triangle::Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2,
                  const std::array<double, 3> normal0, const std::array<double, 3> normal1, const std::array<double, 3> normal2, const std::array<int, 3> color, const Texture* texture, const std::array<int, 2> textureUV0, const std::array<int, 2> textureUV1, const std::array<int, 2> textureUV2, const double h0, const double h1, const double h2)
    : v0(v0), v1(v1), v2(v2), h0(h0), h1(h1), h2(h2), normal0(normal0), normal1(normal1), normal2(normal2), normal(VectorHelper::GetNormal(v0, v1, v2)), color(color), texture(texture), textureUV0(textureUV0), textureUV1(textureUV1), textureUV2(textureUV2)
{
}
Triangle::Triangle(const Triangle& other)
    : v0(other.v0), v1(other.v1), v2(other.v2), color(other.color), h0(other.h0), h1(other.h1), h2(other.h2), normal(other.normal), normal0(other.normal0), normal1(other.normal1), normal2(other.normal2), texture(other.texture), textureUV0(other.textureUV0), textureUV1(other.textureUV1), textureUV2(other.textureUV2)
{
}

Triangle::~Triangle()
{
}

const std::array<double, 3>& Triangle::GetV0() const
{
    return v0;
}

const std::array<double, 3>& Triangle::GetV1() const
{
    return v1;
}

const std::array<double, 3>& Triangle::GetV2() const
{
    return v2;
}

const std::array<int, 3>& Triangle::GetColor() const
{
    return color;
}

const double Triangle::GetH0() const
{
    return h0;
}

const double Triangle::GetH1() const
{
    return h1;
}

const double Triangle::GetH2() const
{
    return h2;
}

const std::array<double, 3>& Triangle::GetNormal0() const
{
    return normal0;
}

const std::array<double, 3>& Triangle::GetNormal1() const
{
    return normal1;
}

const std::array<double, 3>& Triangle::GetNormal2() const
{
    return normal2;
}

const std::array<double, 3>& Triangle::GetNormal() const
{
    return normal;
}

void Triangle::SetH0(const double h0)
{
    this->h0 = h0;
}

void Triangle::SetH1(const double h1)
{
    this->h1 = h1;
}

void Triangle::SetH2(const double h2)
{
    this->h2 = h2;
}

void Triangle::SetNormal0(const std::array<double, 3> normal0)
{
    this->normal0 = normal0;
}

void Triangle::SetNormal1(const std::array<double, 3> normal1)
{
    this->normal1 = normal1;
}

void Triangle::SetNormal2(const std::array<double, 3> normal2)
{
    this->normal2 = normal2;
}

const Texture* Triangle::GetTexture() const
{
    return texture;
}

const std::array<int, 2>& Triangle::GetTextureUV0() const
{
    return textureUV0;
}

const std::array<int, 2>& Triangle::GetTextureUV1() const
{
    return textureUV1;
}

const std::array<int, 2>& Triangle::GetTextureUV2() const
{
    return textureUV2;
}
