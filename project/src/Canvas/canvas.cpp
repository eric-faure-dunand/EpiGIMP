#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "canvas.hpp"

namespace gimp {

Canvas::Canvas(int width, int height) : Width(width), Height(height) {
    Buffer.resize(static_cast<size_t>(Width) * Height * 4, 200);

    glGenTextures(1, &TextureId);
    glBindTexture(GL_TEXTURE_2D, TextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    UpdateTexture();
}

Canvas::~Canvas() {
    if (TextureId)
        glDeleteTextures(1, &TextureId);
}

void Canvas::UpdateTexture() {
    glBindTexture(GL_TEXTURE_2D, TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffer.data());
}

void Canvas::LoadFromFile(const std::string& path) {
    int w, h, channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4); // force 4 canaux (RGBA)

    if (!data)
        throw FileNotFound(path);

    Width = w;
    Height = h;
    Buffer.assign(data, data + (static_cast<size_t>(w) * h * 4));
    stbi_image_free(data);

    UpdateTexture();
}

}