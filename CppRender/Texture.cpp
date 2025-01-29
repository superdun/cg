#define NOMINMAX
#include "Texture.h"
#include <cmath>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <algorithm> 

Texture::Texture(const std::string& filename):filename(filename)
{
    if (filename.find(".jpg") != std::string::npos) {
        loadJpg();
    }
    else {
        throw std::runtime_error("Unsupported file format");
    }
}

Texture::~Texture() {}

const int Texture::GetWidth() const {
    return width;
}

const int Texture::GetHeight() const {
    return height;
}


const std::array<int, 3>& Texture::GetPixel(const double u, const double v) const {
     int x = u * width;
     int y = v * height;
     x = std::min(x, width - 1);
     y = std::min(y, height - 1);
    return texture[y][x];
}

void Texture::loadJpg() {
    
    // Load the image
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, nullptr, 3);
    if (!data) {
        throw std::runtime_error("Error loading JPEG file: " + filename);
    }

    // Resize the texture vector to hold the image data
    texture.resize(height, std::vector<std::array<int, 3>>(width));

    // Fill the texture with pixel data
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            texture[y][x] = { data[index], data[index + 1], data[index + 2] }; // Set Red, Green, Blue
        }
       
    }
    stbi_image_free(data);
}

