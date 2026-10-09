#include "icon.hpp"

#include "stb_image.h"
#include "FileNotFound.hpp"

namespace gimp {

Icon::Icon(const std::string& path, bool monochrome) {
    int channels = 0;
    unsigned char* pixels = stbi_load(path.c_str(), &Width, &Height, &channels, 4);
    if (!pixels)
        throw FileNotFound(path);

    if (monochrome) {
        size_t count = static_cast<size_t>(Width) * Height;
        for (size_t i = 0; i < count; i++) {
            pixels[i * 4 + 0] = 255;
            pixels[i * 4 + 1] = 255;
            pixels[i * 4 + 2] = 255;
        }
    }

    glGenTextures(1, &TextureId);
    glBindTexture(GL_TEXTURE_2D, TextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);
}

Icon::~Icon() {
    if (TextureId)
        glDeleteTextures(1, &TextureId);
}

}
