#ifndef LAYER_HPP
    #define LAYER_HPP

    #include <cstdint>
    #include <string>
    #include <vector>

    #include "FileNotFound.hpp"
    #include "stb_image.h"

namespace gimp {

class Layer {
    int Width;
    int Height;
    std::vector<uint8_t> Buffer;
    bool Visible = true;
    std::string Name;

public:
    Layer(int width, int height, const std::string& name);

    void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void DrawBrush(int centerX, int centerY, int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void LoadFromFile(const std::string& path);

    int getWidth() const {return Width;};
    int getHeight() const {return Height;};
    bool isVisible() const {return Visible;};
    void setVisible(bool visible) {Visible = visible;};
    const std::string& getName() const {return Name;};
    void setName(const std::string& name) {Name = name;};
    const std::vector<uint8_t>& getBuffer() const {return Buffer;};
};

}

#endif