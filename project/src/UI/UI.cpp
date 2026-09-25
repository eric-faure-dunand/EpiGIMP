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

    MyDocument = std::make_unique<Document>(1, 1);
    MyDocument->LoadBaseLayerFromFile(imagePath);

    MyCanvas = std::make_unique<Canvas>(MyDocument->getWidth(), MyDocument->getHeight());
    MyCanvas->UpdateTexture(MyDocument->Composite());
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

uint8_t UI::NormalizeColor(int value) {
    if (value < 0)
        return 0;
    if (value > 255)
        return 255;
    return static_cast<uint8_t>(value);
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

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
                            | ImGuiWindowFlags_NoResize
                            | ImGuiWindowFlags_NoCollapse
                            | ImGuiWindowFlags_NoTitleBar
                            | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Canvas", nullptr, flags);

    ImGui::Image(
        (ImTextureID)(intptr_t)MyCanvas->getTextureId(),
        ImVec2((float)MyCanvas->getWidth(), (float)MyCanvas->getHeight())
    );
    UpdateCanvasMouseInput();

    if (MouseInCanvas && (MouseDown || MouseDragging)) {
        uint8_t r = NormalizeColor(BrushColor[0]);
        uint8_t g = NormalizeColor(BrushColor[1]);
        uint8_t b = NormalizeColor(BrushColor[2]);

        Layer* active = MyDocument->getActiveLayer();
        if (active) {
            active->DrawBrush(MousePixelX, MousePixelY, BrushSize, r, g, b, 255);
            MyCanvas->UpdateTexture(MyDocument->Composite());
        }
    }

    ImGui::SliderInt3("Couleur (RGB)", BrushColor, 0, 255, "%d", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SameLine();
    ImVec4 previewColor(BrushColor[0] / 255.0f, BrushColor[1] / 255.0f, BrushColor[2] / 255.0f, 1.0f);
    ImGui::ColorButton("Apercu couleur", previewColor, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(24, 24));
    ImGui::SliderInt("Taille pinceau", &BrushSize, 1, 50);

    ImGui::Separator();
    ImGui::InputText("Export path", ExportPathBuffer, sizeof(ExportPathBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Export")) {
        try {
            MyDocument->SaveToFile(ExportPathBuffer);
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
