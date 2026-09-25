#ifndef CANVAS_HPP
    #define CANVAS_HPP

    #include <cstdint>
    #include <vector>
    #include <glad/gl.h>

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

    int getWidth() const {return Width;};
    int getHeight() const {return Height;};
    GLuint getTextureId() const {return TextureId;};
    std::vector<uint8_t>& getBuffer() {return Buffer;};
};

}

#endif