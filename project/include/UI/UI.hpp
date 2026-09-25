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

namespace gimp {
class UI {
    GLFWwindow* Window = nullptr;
    bool ShouldClose = false;
    std::unique_ptr<Canvas> MyCanvas;
    std::unique_ptr<Document> MyDocument;

    char ExportPathBuffer[256] = "output.png";
    std::string ExportStatus;

    bool MouseInCanvas = false;
    bool MouseDown = false;
    bool MouseDragging = false;
    int MousePixelX = 0;
    int MousePixelY = 0;

    int BrushColor[3] = {0, 0, 0};
    int BrushSize = 4;

    int LayerCounter = 1;

    uint8_t NormalizeColor(int value);

public:
    UI(const std::string& imagePath);
    ~UI();

    void DrawFrame();
    void UpdateCanvasMouseInput();
    void DrawLayerPanel();

    bool open() {return !ShouldClose;};
};

}

#endif