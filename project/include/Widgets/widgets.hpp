#ifndef WIDGETS_HPP
    #define WIDGETS_HPP

    #include "imgui.h"
    #include "icon.hpp"

namespace gimp::Widgets {

bool IconButton(const char* id, const Icon* icon, const char* fallbackLabel, float iconSize, bool active = false);
void RichTooltip(const char* title, const char* shortcut = nullptr, const char* description = nullptr);
void Hint(const char* text);
void BeginRibbonGroup(float offsetY = 0.0f);
void EndRibbonGroup(const char* label, float rowHeight);
void VerticalSeparator(float height);
float VerticalSplitter(const char* id, float width, float height);

}

#endif
