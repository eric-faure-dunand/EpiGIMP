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

    bool Draw(const char* id, float swatchSize, float spacing, int (&target)[3]) const;

    int getSwatchCount() const {return static_cast<int>(Swatches.size());};
    ColorSwatch& getSwatch(int index) {return Swatches[index];};
};

}

#endif
