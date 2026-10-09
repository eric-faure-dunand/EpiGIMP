#include "filter.hpp"

namespace gimp {

void Filter::Grayscale(Layer& layer) {
    std::vector<uint8_t> buffer = layer.getBuffer();

    for (size_t i = 0; i < buffer.size(); i += 4) {
        int gray = (299 * buffer[i] + 587 * buffer[i + 1] + 114 * buffer[i + 2]) / 1000;

        buffer[i + 0] = static_cast<uint8_t>(gray);
        buffer[i + 1] = static_cast<uint8_t>(gray);
        buffer[i + 2] = static_cast<uint8_t>(gray);
        // buffer[i + 3] (alpha) ne change pas
    }

    layer.setBuffer(buffer);
}

}