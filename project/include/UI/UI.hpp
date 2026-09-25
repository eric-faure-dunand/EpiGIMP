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

public:
    UI(const std::string& imagePath);
    ~UI();

    void DrawFrame();

    bool open() {return !ShouldClose;};
};

}

#endif