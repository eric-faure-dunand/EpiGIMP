#include "theme.hpp"

namespace gimp::Theme {

void Apply() {
    ImGui::StyleColorsLight();

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.PopupRounding = 3.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                 = Rgb(0x1A1A1A);
    colors[ImGuiCol_TextDisabled]         = Rgb(0x707070);
    colors[ImGuiCol_WindowBg]             = PanelBg;
    colors[ImGuiCol_PopupBg]              = Rgb(0xFFFFFF);
    colors[ImGuiCol_Border]               = Rgb(0xCFCFCF);
    colors[ImGuiCol_FrameBg]              = Rgb(0xE3E3E3);
    colors[ImGuiCol_FrameBgHovered]       = Rgb(0xD5D5D5);
    colors[ImGuiCol_FrameBgActive]        = AccentLight;
    colors[ImGuiCol_TitleBg]              = Rgb(0xE3E3E3);
    colors[ImGuiCol_TitleBgActive]        = Rgb(0xD5D5D5);
    colors[ImGuiCol_TitleBgCollapsed]     = Rgb(0xE3E3E3);
    colors[ImGuiCol_MenuBarBg]            = BarBg;
    colors[ImGuiCol_ScrollbarBg]          = BarBg;
    colors[ImGuiCol_ScrollbarGrab]        = Rgb(0xC8C8C8);
    colors[ImGuiCol_ScrollbarGrabHovered] = Rgb(0xADADAD);
    colors[ImGuiCol_ScrollbarGrabActive]  = Rgb(0x969696);
    colors[ImGuiCol_CheckMark]            = Accent;
    colors[ImGuiCol_SliderGrab]           = Rgb(0xA0A0A0);
    colors[ImGuiCol_SliderGrabActive]     = Accent;
    colors[ImGuiCol_Button]               = Rgb(0xE3E3E3);
    colors[ImGuiCol_ButtonHovered]        = Rgb(0xD5D5D5);
    colors[ImGuiCol_ButtonActive]         = AccentLight;
    colors[ImGuiCol_Header]               = AccentLight;
    colors[ImGuiCol_HeaderHovered]        = Rgb(0xE5F1FB);
    colors[ImGuiCol_HeaderActive]         = AccentLight;
    colors[ImGuiCol_Separator]            = Rgb(0xCFCFCF);
    colors[ImGuiCol_SeparatorHovered]     = Accent;
    colors[ImGuiCol_SeparatorActive]      = Accent;
    colors[ImGuiCol_InputTextCursor]      = colors[ImGuiCol_Text];
    colors[ImGuiCol_TextSelectedBg]       = AccentLight;
    colors[ImGuiCol_NavCursor]            = Accent;
}

}
