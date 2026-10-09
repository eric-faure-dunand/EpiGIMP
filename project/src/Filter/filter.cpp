#include "filter.hpp"

namespace gimp {

uint8_t Filter::ClampToByte(float value) {
    if (value < 0.0f)
        return 0;
    if (value > 255.0f)
        return 255;
    return static_cast<uint8_t>(value + 0.5f);
}

void Filter::Grayscale(Layer& layer) {
    std::vector<uint8_t> buffer = layer.getBuffer();

    for (size_t i = 0; i < buffer.size(); i += 4) {
        int gray = (299 * buffer[i] + 587 * buffer[i + 1] + 114 * buffer[i + 2]) / 1000;

        buffer[i + 0] = static_cast<uint8_t>(gray);
        buffer[i + 1] = static_cast<uint8_t>(gray);
        buffer[i + 2] = static_cast<uint8_t>(gray);
    }

    layer.setBuffer(buffer);
}

void Filter::Invert(Layer& layer) {
    std::vector<uint8_t> buffer = layer.getBuffer();

    for (size_t i = 0; i < buffer.size(); i += 4) {
        buffer[i + 0] = static_cast<uint8_t>(255 - buffer[i + 0]);
        buffer[i + 1] = static_cast<uint8_t>(255 - buffer[i + 1]);
        buffer[i + 2] = static_cast<uint8_t>(255 - buffer[i + 2]);
    }

    layer.setBuffer(buffer);
}

void Filter::BrightnessContrast(Layer& layer, int brightness, int contrast) {
    std::vector<uint8_t> buffer = layer.getBuffer();

    float offset = brightness * 255.0f / 100.0f;
    float c = contrast * 255.0f / 100.0f;
    float factor = (259.0f * (c + 255.0f)) / (255.0f * (259.0f - c));

    for (size_t i = 0; i < buffer.size(); i += 4) {
        for (int k = 0; k < 3; k++)
            buffer[i + k] = ClampToByte(factor * (buffer[i + k] - 128.0f) + 128.0f + offset);
    }

    layer.setBuffer(buffer);
}

}