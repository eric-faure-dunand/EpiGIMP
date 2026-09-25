#include "document.hpp"

namespace gimp {

Document::Document(int width, int height) : Width(width), Height(height) {
    AddLayer("Layer 1");
}

void Document::LoadBaseLayerFromFile(const std::string& path) {
    auto layer = std::make_unique<Layer>(1, 1, "Background");
    layer->LoadFromFile(path);

    Width = layer->getWidth();
    Height = layer->getHeight();

    Layers.clear();
    Layers.push_back(std::move(layer));
    ActiveLayerIndex = 0;
}

Layer& Document::AddLayer(const std::string& name) {
    Layers.push_back(std::make_unique<Layer>(Width, Height, name));
    ActiveLayerIndex = static_cast<int>(Layers.size()) - 1;
    return *Layers[ActiveLayerIndex];
}

void Document::RemoveLayer(int index) {
    if (index < 0 || index >= static_cast<int>(Layers.size()))
        return;

    Layers.erase(Layers.begin() + index);

    if (Layers.empty()) {
        ActiveLayerIndex = -1;
        return;
    }
    if (ActiveLayerIndex >= static_cast<int>(Layers.size()))
        ActiveLayerIndex = static_cast<int>(Layers.size()) - 1;
}

std::vector<uint8_t> Document::Composite() const {
    std::vector<uint8_t> result(static_cast<size_t>(Width) * Height * 4, 0);

    for (const auto& layerPtr : Layers) {
        if (!layerPtr->isVisible())
            continue;

        const std::vector<uint8_t>& src = layerPtr->getBuffer();

        for (size_t p = 0; p < result.size(); p += 4) {
            uint8_t srcA = src[p + 3];
            if (srcA == 0)
                continue;

            float alpha = srcA / 255.0f;
            for (int c = 0; c < 3; c++) {
                float srcC = src[p + c];
                float dstC = result[p + c];
                result[p + c] = static_cast<uint8_t>(srcC * alpha + dstC * (1.0f - alpha));
            }
            float dstA = result[p + 3] / 255.0f;
            float outA = alpha + dstA * (1.0f - alpha);
            result[p + 3] = static_cast<uint8_t>(outA * 255.0f);
        }
    }

    return result;
}

static std::string toLowerExt(const std::string& path) {
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos)
        return "";
    std::string ext = path.substr(dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
    return ext;
}

void Document::SaveToFile(const std::string& path) const {
    std::vector<uint8_t> flat = Composite();
    std::string ext = toLowerExt(path);
    int stride = Width * 4;
    int ok = 0;

    if (ext == "png")
        ok = stbi_write_png(path.c_str(), Width, Height, 4, flat.data(), stride);
    else if (ext == "bmp")
        ok = stbi_write_bmp(path.c_str(), Width, Height, 4, flat.data());
    else if (ext == "jpg" || ext == "jpeg")
        ok = stbi_write_jpg(path.c_str(), Width, Height, 4, flat.data(), 90);
    else
        throw Error("Format d'export non supporte (extension attendue: .png, .bmp, .jpg): " + path);

    if (!ok)
        throw Error("Echec de l'ecriture du fichier: " + path);
}

Layer* Document::getActiveLayer() {
    if (ActiveLayerIndex < 0 || ActiveLayerIndex >= static_cast<int>(Layers.size()))
        return nullptr;
    return Layers[ActiveLayerIndex].get();
}

}