#include "palette.hpp"

#include <array>
#include <cstdint>

namespace gimp {

Palette::Palette(std::vector<ColorSwatch> swatches, int columns)
    : Swatches(std::move(swatches)), Columns(columns) {}

Palette Palette::Base() {
    static constexpr std::array<uint32_t, 20> colors = {
        0x000000, 0x7F7F7F, 0x880015, 0xED1C24, 0xFF7F27, 0xFFF200, 0x22B14C, 0x00A2E8, 0x3F48CC, 0xA349A4,
        0xFFFFFF, 0xC3C3C3, 0xB97A57, 0xFFAEC9, 0xFFC90E, 0xEFE4B0, 0xB5E61D, 0x99D9EA, 0x7092BE, 0xC8BFE7,
    };

    std::vector<ColorSwatch> swatches;
    for (uint32_t hex : colors) {
        swatches.emplace_back(
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF));
    }
    return Palette(std::move(swatches), 10);
}

bool Palette::Draw(const char* id, float swatchSize, float spacing, int (&target)[3]) const {
    bool changed = false;

    ImGui::PushID(id);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));
    ImGui::BeginGroup();
    for (int i = 0; i < getSwatchCount(); i++) {
        if (i % Columns != 0)
            ImGui::SameLine();
        ImGui::PushID(i);
        if (Swatches[i].Draw("##swatch", swatchSize, target))
            changed = true;
        ImGui::PopID();
    }
    ImGui::EndGroup();
    ImGui::PopStyleVar();
    ImGui::PopID();
    return changed;
}

}
