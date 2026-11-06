#ifndef TOOL_HPP
    #define TOOL_HPP

    #include <array>

    #include "imgui.h"

namespace gimp {

enum class ToolMode {
    Brush,
    Highlighter,
    Pencil,
    Eraser,
    Picker,
    Selection
};

struct ToolInfo {
    ToolMode Mode;
    const char* IconName;
    const char* Label;
    const char* ShortcutLabel;
    ImGuiKeyChord Shortcut;
    const char* Description;
    bool UsesSize;
};

const std::array<ToolInfo, 6>& GetTools();
const ToolInfo& GetToolInfo(ToolMode mode);

}

#endif
