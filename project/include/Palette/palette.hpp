#ifndef PALETTE_HPP
    #define PALETTE_HPP

    #include <vector>

    #include "colorSwatch.hpp"

namespace gimp {

class Palette {
    std::vector<ColorSwatch> Swatches;
    int Columns;

public:
    Palette(std::vector<ColorSwatch> swatches, int columns);

    static Palette Base();
    static Palette Empty(int count, int columns);

    bool Draw(const char* id, float swatchSize, float spacing, int (&target)[3]) const;

    bool contains(uint8_t r, uint8_t g, uint8_t b) const;
    bool AddColor(uint8_t r, uint8_t g, uint8_t b);

    int getSwatchCount() const {return static_cast<int>(Swatches.size());};
    int getRowCount() const {return (getSwatchCount() + Columns - 1) / Columns;};
    ColorSwatch& getSwatch(int index) {return Swatches[index];};
};

}

#endif
