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
    UpdateCanvasMouseInput();

    ImGui::Separator();
    ImGui::InputText("Export path", ExportPathBuffer, sizeof(ExportPathBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Export")) {
        try {
            MyCanvas->SaveToFile(ExportPathBuffer);
            ExportStatus = "Exporte: " + std::string(ExportPathBuffer);
        } catch (const IError& e) {
            ExportStatus = std::string("Erreur: ") + e.what();
        }
    }
    if (!ExportStatus.empty())
        ImGui::TextUnformatted(ExportStatus.c_str());

    ImGui::Separator();
    if (MouseInCanvas)
        ImGui::Text("Pixel: (%d, %d)  %s", MousePixelX, MousePixelY, MouseDragging ? "[drag]" : (MouseDown ? "[clic]" : ""));
    else
        ImGui::TextDisabled("Pixel: hors du canvas");

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

void UI::UpdateCanvasMouseInput() {
    bool hovered = ImGui::IsItemHovered();
    MouseInCanvas = hovered;

    if (!hovered) {
        MouseDown = false;
        MouseDragging = false;
        return;
    }

    ImVec2 imageOrigin = ImGui::GetItemRectMin();
    ImVec2 mousePos = ImGui::GetMousePos();

    float localX = mousePos.x - imageOrigin.x;
    float localY = mousePos.y - imageOrigin.y;

    if (localX < 0.0f) localX = 0.0f;
    if (localY < 0.0f) localY = 0.0f;
    if (localX >= (float)MyCanvas->getWidth())  localX = (float)MyCanvas->getWidth() - 1.0f;
    if (localY >= (float)MyCanvas->getHeight()) localY = (float)MyCanvas->getHeight() - 1.0f;

    MousePixelX = (int)localX;
    MousePixelY = (int)localY;

    bool leftDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    MouseDragging = leftDown && MouseDown;
    MouseDown = leftDown;
}

}
