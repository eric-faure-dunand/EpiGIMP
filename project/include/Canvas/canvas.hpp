#ifndef CANVAS_HPP
    #define CANVAS_HPP

    #include <cstdint>
    #include <vector>
    #include <string>
    #include <glad/gl.h>
    #include <algorithm>
    #include <cctype>

    #include "FileNotFound.hpp"
    #include "Error.hpp"

namespace gimp {

class Canvas {
    int Width;
    int Height;
    std::vector<uint8_t> Buffer;
    GLuint TextureId = 0;

public:
    Canvas(int width, int height);
    ~Canvas();

    void UpdateTexture();
    void LoadFromFile(const std::string& path);
    void SaveToFile(const std::string& path);

    int getWidth() const {return Width;};
    int getHeight() const {return Height;};
    GLuint getTextureId() const {return TextureId;};
    std::vector<uint8_t>& getBuffer() {return Buffer;};
};

}

#endif