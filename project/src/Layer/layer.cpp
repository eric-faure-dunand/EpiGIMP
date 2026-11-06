#include "layer.hpp"

#include <cstdlib>

namespace gimp {

namespace {

template <typename Stamp>
void WalkLine(int fromX, int fromY, int toX, int toY, Stamp stamp) {
    int dx = std::abs(toX - fromX);
    int dy = -std::abs(toY - fromY);
    int stepX = fromX < toX ? 1 : -1;
    int stepY = fromY < toY ? 1 : -1;
    int error = dx + dy;
    int x = fromX;
    int y = fromY;

    while (true) {
        stamp(x, y);
        if (x == toX && y == toY)
            break;

        int doubledError = 2 * error;
        if (doubledError >= dy) {
            error += dy;
            x += stepX;
        }
        if (doubledError <= dx) {
            error += dx;
            y += stepY;
        }
    }
}

}

Layer::Layer(int width, int height, const std::string& name) : Width(width), Height(height), Name(name) {
    Buffer.resize(static_cast<size_t>(Width) * Height * 4, 0);
    Mask.resize(static_cast<size_t>(Width) * Height, 255);   // entierement visible par defaut
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

void Layer::DrawBrush(int centerX, int centerY, int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a,
    bool hasClip, int clipMinX, int clipMinY, int clipMaxX, int clipMaxY) {
    if (radius < 1)
        radius = 1;

    int rSquared = radius * radius;

    for (int y = centerY - radius; y <= centerY + radius; y++) {
        if (hasClip && (y < clipMinY || y > clipMaxY))
            continue;

        for (int x = centerX - radius; x <= centerX + radius; x++) {
            if (hasClip && (x < clipMinX || x > clipMaxX))
                continue;

            int dx = x - centerX;
            int dy = y - centerY;
            if (dx * dx + dy * dy <= rSquared)
                SetPixel(x, y, r, g, b, a);
        }
    }
}

void Layer::DrawBrushLine(int fromX, int fromY, int toX, int toY, int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a,
    bool hasClip, int clipMinX, int clipMinY, int clipMaxX, int clipMaxY) {
    WalkLine(fromX, fromY, toX, toY, [&](int x, int y) {
        DrawBrush(x, y, radius, r, g, b, a, hasClip, clipMinX, clipMinY, clipMaxX, clipMaxY);
    });
}

void Layer::SetMaskPixel(int x, int y, uint8_t value) {
    if (x < 0 || y < 0 || x >= Width || y >= Height)
        return;

    Mask[static_cast<size_t>(y) * Width + x] = value;
}

void Layer::DrawMaskBrush(int centerX, int centerY, int radius, uint8_t value,
    bool hasClip, int clipMinX, int clipMinY, int clipMaxX, int clipMaxY) {
    if (radius < 1)
        radius = 1;

    int rSquared = radius * radius;

    for (int y = centerY - radius; y <= centerY + radius; y++) {
        if (hasClip && (y < clipMinY || y > clipMaxY))
            continue;

        for (int x = centerX - radius; x <= centerX + radius; x++) {
            if (hasClip && (x < clipMinX || x > clipMaxX))
                continue;

            int dx = x - centerX;
            int dy = y - centerY;
            if (dx * dx + dy * dy <= rSquared)
                SetMaskPixel(x, y, value);
        }
    }
}

void Layer::DrawMaskBrushLine(int fromX, int fromY, int toX, int toY, int radius, uint8_t value,
    bool hasClip, int clipMinX, int clipMinY, int clipMaxX, int clipMaxY) {
    WalkLine(fromX, fromY, toX, toY, [&](int x, int y) {
        DrawMaskBrush(x, y, radius, value, hasClip, clipMinX, clipMinY, clipMaxX, clipMaxY);
    });
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

    Mask.assign(static_cast<size_t>(Width) * Height, 255);   // re-init masque a la taille de la nouvelle image
}

}