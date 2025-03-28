#pragma once
#include <array>
#include "Texture.h"
class Triangle
{
private:
    const std::array<double, 3> v0;
    const std::array<double, 3> v1;
    const std::array<double, 3> v2;
    double h0;
    double h1;
    double h2;
    std::array<double, 3> normal0;
    std::array<double, 3> normal1;
    std::array<double, 3> normal2;
    const std::array<int, 3> color;
    const std::array<double, 3> normal;
    const Texture* texture;
    std::array<int, 2> textureUV0;
    std::array<int, 2> textureUV1;
    std::array<int, 2> textureUV2;

public:
    Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2,
const std::array<double, 3> normal0, const std::array<double, 3> normal1, const std::array<double, 3> normal2, const std::array<int, 3> color);
    Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2, const std::array<int, 3> color, const Texture* texture, const std::array<int, 2> textureUV0, const std::array<int, 2> textureUV1, const std::array<int, 2> textureUV2);
    Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2, const std::array<int, 3> color, const Texture* texture, const std::array<int, 2> textureUV0, const std::array<int, 2> textureUV1, const std::array<int, 2> textureUV2, const double h0, const double h1, const double h2);
    Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2,
const std::array<double, 3> normal0, const std::array<double, 3> normal1, const std::array<double, 3> normal2, const std::array<int, 3> color, const Texture* texture, const std::array<int, 2> textureUV0, const std::array<int, 2> textureUV1, const std::array<int, 2> textureUV2);
     Triangle(const std::array<double, 3> v0, const std::array<double, 3> v1, const std::array<double, 3> v2,
     const std::array<double, 3> normal0, const std::array<double, 3> normal1, const std::array<double, 3> normal2, const std::array<int, 3> color, const Texture* texture, const std::array<int, 2> textureUV0, const std::array<int, 2> textureUV1, const std::array<int, 2> textureUV2, const double h0, const double h1, const double h2);
    Triangle(const Triangle &other);
    const std::array<double, 3> &GetV0() const;
    const std::array<double, 3> &GetV1() const;
    const std::array<double, 3> &GetV2() const;
    const std::array<int, 3> &GetColor() const;
    const double GetH0() const;
    const double GetH1() const;
    const double GetH2() const;
    const std::array<double, 3> &GetNormal0() const;
    const std::array<double, 3> &GetNormal1() const;
    const std::array<double, 3> &GetNormal2() const;
    const std::array<double, 3> &GetNormal() const;
    ~Triangle();
    void SetH0(const double h0);
    void SetH1(const double h1);
    void SetH2(const double h2);
    void SetNormal0(const std::array<double, 3> normal0);
    void SetNormal1(const std::array<double, 3> normal1);
    void SetNormal2(const std::array<double, 3> normal2);
    const Texture* GetTexture() const;
    const std::array<int, 2>& GetTextureUV0() const;
    const std::array<int, 2>& GetTextureUV1() const;
    const std::array<int, 2>& GetTextureUV2() const;
};
