#ifndef DOCUMENT_HPP
    #define DOCUMENT_HPP

    #include <cstdint>
    #include <memory>
    #include <string>
    #include <vector>
    #include <algorithm>
    #include <cctype>

    #include "layer.hpp"
    #include "Error.hpp"
    #include "stb_image_write.h"


namespace gimp {

class Document {
    int Width;
    int Height;
    std::vector<std::unique_ptr<Layer>> Layers;
    int ActiveLayerIndex = -1;

public:
    Document(int width, int height);

    void LoadBaseLayerFromFile(const std::string& path);
    void SaveToFile(const std::string& path) const;

    Layer& AddLayer(const std::string& name);
    void RemoveLayer(int index);

    std::vector<uint8_t> Composite() const;

    Layer* getActiveLayer();
    void setActiveLayerIndex(int index) {ActiveLayerIndex = index;};
    int getActiveLayerIndex() const {return ActiveLayerIndex;};

    int getLayerCount() const {return static_cast<int>(Layers.size());};
    Layer& getLayer(int index) {return *Layers[index];};

    int getWidth() const {return Width;};
    int getHeight() const {return Height;};
};

}

#endif