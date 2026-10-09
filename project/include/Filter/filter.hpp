#ifndef FILTER_HPP
    #define FILTER_HPP
    #include <cstdint>
    #include <vector>

    #include "layer.hpp"

namespace gimp {

class Filter {
public:
    static void Grayscale(Layer& layer);
};

}

#endif