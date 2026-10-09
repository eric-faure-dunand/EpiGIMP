#ifndef FILTER_HPP
    #define FILTER_HPP
    #include <cstdint>
    #include <vector>

    #include "layer.hpp"

namespace gimp {

class Filter {
    static uint8_t ClampToByte(float);
public:
    static void Grayscale(Layer& layer);
    static void Invert(Layer& layer);
    static void BrightnessContrast(Layer& layer, int brightness, int contrast);
};

}

#endif