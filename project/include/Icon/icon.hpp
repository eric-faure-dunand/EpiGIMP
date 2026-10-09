#ifndef ICON_HPP
    #define ICON_HPP

    #include <string>
    #include <glad/gl.h>

namespace gimp {

class Icon {
    GLuint TextureId = 0;
    int Width = 0;
    int Height = 0;

public:
    Icon(const std::string& path, bool monochrome = true);
    ~Icon();

    Icon(const Icon&) = delete;
    Icon& operator=(const Icon&) = delete;

    GLuint getTextureId() const {return TextureId;};
    int getWidth() const {return Width;};
    int getHeight() const {return Height;};
};

}

#endif
