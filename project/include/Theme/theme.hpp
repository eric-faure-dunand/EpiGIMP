#ifndef THEME_HPP
    #define THEME_HPP

    #include "imgui.h"

namespace gimp::Theme {

constexpr ImVec4 Rgb(unsigned int hex, float alpha = 1.0f) {
    return ImVec4(((hex >> 16) & 0xFF) / 255.0f, ((hex >> 8) & 0xFF) / 255.0f, (hex & 0xFF) / 255.0f, alpha);
}

inline constexpr ImVec4 BarBg = Rgb(0xF0F0F0);
inline constexpr ImVec4 PanelBg = Rgb(0xF7F7F7);
inline constexpr ImVec4 CanvasBg = Rgb(0xC4C4C4);
inline constexpr ImVec4 Accent = Rgb(0x6A9FD4);
inline constexpr ImVec4 AccentLight = Rgb(0xCCE4F7);
inline constexpr ImVec4 MaskWarning = Rgb(0xB86E00);

void Apply();

}

#endif
