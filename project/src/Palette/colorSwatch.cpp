#include "colorSwatch.hpp"

namespace gimp {

ColorSwatch::ColorSwatch(uint8_t r, uint8_t g, uint8_t b) : Color(std::array<uint8_t, 3>{r, g, b}) {}

bool ColorSwatch::Draw(const char* id, float size, int (&target)[3]) const {
    bool pressed = false;

    if (Color) {
        const std::array<uint8_t, 3>& rgb = *Color;
        ImVec4 color(rgb[0] / 255.0f, rgb[1] / 255.0f, rgb[2] / 255.0f, 1.0f);
        pressed = ImGui::ColorButton(id, color, ImGuiColorEditFlags_NoAlpha, ImVec2(size, size));
        if (pressed) {
            for (int i = 0; i < 3; i++)
                target[i] = rgb[i];
        }
    } else {
        ImGui::Dummy(ImVec2(size, size));
    }

    ImGuiCol border = (Color && ImGui::IsItemHovered()) ? ImGuiCol_SeparatorHovered : ImGuiCol_Border;
    ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
        ImGui::GetColorU32(border), ImGui::GetStyle().FrameRounding);
    return pressed;
}

}
