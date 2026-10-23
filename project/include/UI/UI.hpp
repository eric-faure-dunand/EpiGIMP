#ifndef UI_HPP
    #define UI_HPP

    #include <memory>
    #include <string>

    #include <glad/gl.h>
    #include <GLFW/glfw3.h>

    #include "imgui.h"
    #include "imgui_impl_glfw.h"
    #include "imgui_impl_opengl3.h"
    #include "Error.hpp"
    #include "canvas.hpp"
    #include "document.hpp"
    #include "filter.hpp"
    #include "history.hpp"
    #include "iconLibrary.hpp"
    #include "theme.hpp"
    #include "tool.hpp"
    #include "widgets.hpp"

namespace gimp {

class UI {
    static constexpr float ToolIconSize = 24.0f;
    static constexpr int BrushSizeMin = 1;
    static constexpr int BrushSizeMax = 50;
    static constexpr std::size_t HistoryDepth = 15;
    static constexpr float ColorFieldWidth = 200.0f;
    static constexpr float SizeSliderWidth = 160.0f;
    static constexpr float SidePanelMinWidth = 200.0f;
    static constexpr float CanvasMinWidth = 200.0f;
    static constexpr float SplitterWidth = 6.0f;
    static constexpr ImVec2 StatusBarPadding = ImVec2(8.0f, 4.0f);
    static constexpr ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoMove
                                                 | ImGuiWindowFlags_NoResize
                                                 | ImGuiWindowFlags_NoCollapse
                                                 | ImGuiWindowFlags_NoTitleBar
                                                 | ImGuiWindowFlags_NoBringToFrontOnFocus
                                                 | ImGuiWindowFlags_NoScrollbar
                                                 | ImGuiWindowFlags_NoScrollWithMouse;

    GLFWwindow* Window = nullptr;
    bool ShouldClose = false;
    std::unique_ptr<Canvas> MyCanvas;
    std::unique_ptr<Document> MyDocument;
    std::unique_ptr<IconLibrary> Icons;

    char ExportPathBuffer[256] = "output.png";
    std::string StatusMessage;
    float SidePanelWidth = 260.0f;

    int FilterBrightness = 0;
    int FilterContrast = 0;
    bool OpenBrightnessPopup = false;

    bool MouseInCanvas = false;
    bool MouseDown = false;
    bool StrokeStarted = false;
    bool MouseDragging = false;
    int MousePixelX = 0;
    int MousePixelY = 0;

    int BrushColor[3] = {0, 0, 0};
    int BrushSize = 4;
    ToolMode CurrentTool = ToolMode::Brush;
    bool EditingMask = false;

    int LayerCounter = 1;

    std::unique_ptr<History> MyHistory;

    bool HasSelection = false;
    int SelectionStartX = 0;
    int SelectionStartY = 0;
    int SelectionMinX = 0;
    int SelectionMinY = 0;
    int SelectionMaxX = 0;
    int SelectionMaxY = 0;

    uint8_t NormalizeColor(int value);
    void LoadIcons();
    void HandleShortcuts();
    float ToolbarRowHeight() const;

public:
    UI(const std::string& imagePath);
    ~UI();

    void DrawFrame();
    void DrawWorkspace();
    void DrawCanvas();
    void UpdateCanvasMouseInput();
    void DrawLayerPanel();
    void DrawExportPanel();
    void DrawToolbar();
    void DrawStatusBar();
    void DrawMenuBar();

    void SaveUndoSnapshot();

    bool open() {return !ShouldClose;};
};

}

#endif