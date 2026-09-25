#include "canvas.hpp"

namespace gimp {

Canvas::Canvas(int width, int height) : Width(width), Height(height) {
    glGenTextures(1, &TextureId);
    glBindTexture(GL_TEXTURE_2D, TextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    std::vector<uint8_t> empty(static_cast<size_t>(Width) * Height * 4, 0);
    UpdateTexture(empty);
}

Canvas::~Canvas() {
    if (TextureId)
        glDeleteTextures(1, &TextureId);
}

void Canvas::Resize(int width, int height) {
    Width = width;
    Height = height;
}

void Canvas::UpdateTexture(const std::vector<uint8_t>& buffer) {
    glBindTexture(GL_TEXTURE_2D, TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
}

}