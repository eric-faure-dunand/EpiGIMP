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

}