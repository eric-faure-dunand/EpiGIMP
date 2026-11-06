#include "tool.hpp"

namespace gimp {

const std::array<ToolInfo, 5>& GetTools() {
    static const std::array<ToolInfo, 5> tools = {{
        {ToolMode::Brush, "brush", "Pinceau", "P", ImGuiKey_P,
            "Peint avec la couleur courante sur le calque actif. "
            "En mode masque : rend la zone peinte visible.", true},
        {ToolMode::Highlighter, "highlighter", "Stabilo", "H", ImGuiKey_H,
            "Peint avec la couleur courante en parallelogramme : largeur double de la taille, hauteur egale a la taille. "
            "En mode masque : rend la zone peinte visible.", true},
        {ToolMode::Eraser, "eraser", "Gomme", "Shift+E", ImGuiMod_Shift | ImGuiKey_E,
            "Efface les pixels du calque actif (transparence). "
            "En mode masque : cache la zone peinte.", true},
        {ToolMode::Picker, "picker", "Pipette", "O", ImGuiKey_O,
            "Cliquez sur l'image pour reprendre sa couleur comme couleur de pinceau.", false},
        {ToolMode::Selection, "selection", "Selection rectangulaire", "R", ImGuiKey_R,
            "Glissez pour tracer un rectangle : le pinceau et la gomme n'agiront qu'a l'interieur.", false},
    }};
    return tools;
}

const ToolInfo& GetToolInfo(ToolMode mode) {
    for (const ToolInfo& tool : GetTools()) {
        if (tool.Mode == mode)
            return tool;
    }
    return GetTools()[0];
}

}
