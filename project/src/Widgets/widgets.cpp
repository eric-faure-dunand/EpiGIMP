#include "widgets.hpp"

#include <cstdint>

namespace gimp::Widgets {

bool IconButton(const char* id, const Icon* icon, const char* fallbackLabel, float iconSize, bool active) {
    const ImGuiStyle& style = ImGui::GetStyle();

    if (active) {
        ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_ButtonActive]);
    }

    bool pressed = false;
    if (icon) {
        pressed = ImGui::ImageButton(id,
            (ImTextureID)(intptr_t)icon->getTextureId(),
            ImVec2(iconSize, iconSize),
            ImVec2(0, 0), ImVec2(1, 1),
            ImVec4(0, 0, 0, 0),
            style.Colors[ImGuiCol_Text]);
    } else {
        ImGui::PushID(id);
        pressed = ImGui::Button(fallbackLabel, ImVec2(0, iconSize + style.FramePadding.y * 2.0f));
        ImGui::PopID();
    }

    if (active) {
        ImGui::PopStyleColor(2);
        ImGui::GetWindowDrawList()->AddRect(
            ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
            ImGui::GetColorU32(ImGuiCol_CheckMark), style.FrameRounding, 0, 2.0f);
    }
    return pressed;
}

void RichTooltip(const char* title, const char* shortcut, const char* description) {
    if (!ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled))
        return;
    if (!ImGui::BeginTooltip())
        return;

    ImGui::TextUnformatted(title);
    if (shortcut && *shortcut) {
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyle().Colors[ImGuiCol_CheckMark], "[%s]", shortcut);
    }
    if (description && *description) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 22.0f);
        ImGui::TextUnformatted(description);
        ImGui::PopTextWrapPos();
    }
    ImGui::EndTooltip();
}

void Hint(const char* text) {
    ImGui::PushTextWrapPos(0.0f);
    ImGui::TextDisabled("%s", text);
    ImGui::PopTextWrapPos();
}

}
