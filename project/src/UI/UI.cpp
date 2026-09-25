#include "UI.hpp"

namespace gimp {

UI::UI(const std::string& imagePath) {
    if (!glfwInit())
        throw Error("Impossible d'initialiser GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window = glfwCreateWindow(1280, 720, "EpiGIMP", nullptr, nullptr);
    if (!Window) {
        glfwTerminate();
        throw Error("Impossible de creer la fenetre GLFW");
    }

    glfwMakeContextCurrent(Window);
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress))
        throw Error("Impossible de charger les fonctions OpenGL (glad)");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    MyCanvas = std::make_unique<Canvas>(1, 1);
    MyCanvas->LoadFromFile(imagePath);
}

UI::~UI() {
    MyCanvas.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (Window)
        glfwDestroyWindow(Window);
    glfwTerminate();
}

void UI::DrawFrame() {
    if (glfwWindowShouldClose(Window)) {
        ShouldClose = true;
        return;
    }

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Canvas");
    ImGui::Image(
        (ImTextureID)(intptr_t)MyCanvas->getTextureId(),
        ImVec2((float)MyCanvas->getWidth(), (float)MyCanvas->getHeight())
    );
    ImGui::End();
    ImGui::Render();

    int w, h;
    glfwGetFramebufferSize(Window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(Window);
}

}
