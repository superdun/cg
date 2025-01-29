#pragma once
#include <vector>
#include <string>
#include <array>
class Texture
{
    std::vector<std::vector<std::array<int, 3>>> texture;
    int width;
    int height;
    const std::string filename;
    void loadJpg();
public:
    Texture(const std::string& filename);
    ~Texture();
    const int GetWidth() const;
    const int GetHeight() const;
    const std::array<int, 3>& GetPixel(const int x, const int y) const;
    const std::array<int, 3>& GetPixel(const double u, const double v) const;
};

