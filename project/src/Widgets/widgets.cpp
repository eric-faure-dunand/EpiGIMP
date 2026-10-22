#include "widgets.hpp"

#include <algorithm>
#include <cmath>
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

void BeginRibbonGroup(float offsetY) {
    ImGui::BeginGroup();
    ImGui::BeginGroup();
    if (offsetY > 0.0f) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0f));
        ImGui::Dummy(ImVec2(0.0f, offsetY));
        ImGui::PopStyleVar();
    }
}

void EndRibbonGroup(const char* label, float rowHeight) {
    ImGui::EndGroup();

    ImVec2 contentMin = ImGui::GetItemRectMin();
    float contentWidth = ImGui::GetItemRectSize().x;
    float labelWidth = ImGui::CalcTextSize(label).x;

    ImGui::SetCursorScreenPos(ImVec2(
        contentMin.x + std::floor(std::max(0.0f, (contentWidth - labelWidth) * 0.5f)),
        contentMin.y + rowHeight + ImGui::GetStyle().ItemSpacing.y));
    ImGui::TextDisabled("%s", label);
    ImGui::EndGroup();
}

void VerticalSeparator(float height) {
    const float spacing = ImGui::GetStyle().ItemSpacing.x * 2.0f;

    ImGui::SameLine(0.0f, spacing);
    ImVec2 top = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddRectFilled(top, ImVec2(top.x + 1.0f, top.y + height),
        ImGui::GetColorU32(ImGuiCol_Separator));
    ImGui::Dummy(ImVec2(1.0f, height));
    ImGui::SameLine(0.0f, spacing);
}

float VerticalSplitter(const char* id, float width, float height) {
    ImGui::InvisibleButton(id, ImVec2(width, height));
    bool active = ImGui::IsItemActive();
    bool hot = active || ImGui::IsItemHovered();
    if (hot)
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

    ImVec2 min = ImGui::GetItemRectMin();
    float lineWidth = hot ? 2.0f : 1.0f;
    float x = std::floor(min.x + (width - lineWidth) * 0.5f);
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, min.y), ImVec2(x + lineWidth, min.y + height),
        ImGui::GetColorU32(hot ? ImGuiCol_SeparatorHovered : ImGuiCol_Separator));

    return active ? ImGui::GetIO().MouseDelta.x : 0.0f;
}

}
