#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);

    if (!data)
        throw FileNotFound(path);

    Width = w;
    Height = h;
    Buffer.assign(data, data + (static_cast<size_t>(w) * h * 4));
    stbi_image_free(data);

    UpdateTexture();
}

static std::string toLowerExt(const std::string& path) {
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos)
        return "";
    std::string ext = path.substr(dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
    return ext;
}

void Canvas::SaveToFile(const std::string& path) {
    std::string ext = toLowerExt(path);
    int stride = Width * 4;
    int ok = 0;

    if (ext == "png")
        ok = stbi_write_png(path.c_str(), Width, Height, 4, Buffer.data(), stride);
    else if (ext == "bmp")
        ok = stbi_write_bmp(path.c_str(), Width, Height, 4, Buffer.data());
    else if (ext == "jpg" || ext == "jpeg")
        ok = stbi_write_jpg(path.c_str(), Width, Height, 4, Buffer.data(), 90);
    else
        throw Error("Format d'export non supporte (extension attendue: .png, .bmp, .jpg): " + path);

    if (!ok)
        throw Error("Echec de l'ecriture du fichier: " + path);
}

}