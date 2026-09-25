#ifndef UI_HPP
    #define UI_HPP

    #include <memory>

    #include <glad/gl.h>
    #include <GLFW/glfw3.h>

    #include "imgui.h"
    #include "imgui_impl_glfw.h"
    #include "imgui_impl_opengl3.h"
    #include "Error.hpp"
    #include "canvas.hpp"

    #define GLFW_INCLUDE_NONE

namespace gimp {

class UI {
    GLFWwindow* Window = nullptr;
    bool ShouldClose = false;
    std::unique_ptr<Canvas> MyCanvas;

    char ExportPathBuffer[256] = "output.png";
    std::string ExportStatus;

    bool MouseInCanvas = false;
    bool MouseDown = false;
    bool MouseDragging = false;
    int MousePixelX = 0;
    int MousePixelY = 0;

public:
    UI(const std::string& imagePath);
    ~UI();

    void UpdateCanvasMouseInput();
    void DrawFrame();

    bool open() {return !ShouldClose;};
};

}

#endif