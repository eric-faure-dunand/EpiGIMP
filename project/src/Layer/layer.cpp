#include "layer.hpp"

namespace gimp {

Layer::Layer(int width, int height, const std::string& name) : Width(width), Height(height), Name(name) {
    Buffer.resize(static_cast<size_t>(Width) * Height * 4, 0);
}

void Layer::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (x < 0 || y < 0 || x >= Width || y >= Height)
        return;

    size_t idx = (static_cast<size_t>(y) * Width + x) * 4;
    Buffer[idx + 0] = r;
    Buffer[idx + 1] = g;
    Buffer[idx + 2] = b;
    Buffer[idx + 3] = a;
}

void Layer::DrawBrush(int centerX, int centerY, int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (radius < 1)
        radius = 1;

    int rSquared = radius * radius;

    for (int y = centerY - radius; y <= centerY + radius; y++) {
        for (int x = centerX - radius; x <= centerX + radius; x++) {
            int dx = x - centerX;
            int dy = y - centerY;
            if (dx * dx + dy * dy <= rSquared)
                SetPixel(x, y, r, g, b, a);
        }
    }
}

void Layer::LoadFromFile(const std::string& path) {
    int w, h, channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);

    if (!data)
        throw FileNotFound(path);

    Width = w;
    Height = h;
    Buffer.assign(data, data + (static_cast<size_t>(w) * h * 4));
    stbi_image_free(data);
}

}