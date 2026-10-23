#ifndef COLOR_SWATCH_HPP
    #define COLOR_SWATCH_HPP

    #include <array>
    #include <cstdint>
    #include <optional>

    #include "imgui.h"

namespace gimp {

class ColorSwatch {
    std::optional<std::array<uint8_t, 3>> Color;

public:
    ColorSwatch() = default;
    ColorSwatch(uint8_t r, uint8_t g, uint8_t b);

    bool Draw(const char* id, float size, int (&target)[3]) const;

    bool hasColor() const {return Color.has_value();};
    bool matches(uint8_t r, uint8_t g, uint8_t b) const {return Color == std::array<uint8_t, 3>{r, g, b};};
    void setColor(uint8_t r, uint8_t g, uint8_t b) {Color = std::array<uint8_t, 3>{r, g, b};};
    void clear() {Color.reset();};
};

}

#endif
