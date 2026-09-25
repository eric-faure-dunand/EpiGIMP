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

    if (HasSelection) {
        ImVec2 imageOrigin = ImGui::GetItemRectMin();
        ImVec2 rectMin(imageOrigin.x + SelectionMinX, imageOrigin.y + SelectionMinY);
        ImVec2 rectMax(imageOrigin.x + SelectionMaxX + 1, imageOrigin.y + SelectionMaxY + 1);
        ImGui::GetWindowDrawList()->AddRect(rectMin, rectMax, IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f);
    }

    UpdateCanvasMouseInput();

    if (MouseInCanvas && (MouseDown || MouseDragging)) {
        if (CurrentTool == ToolMode::Selection) {
            if (StrokeStarted) {
                SelectionStartX = MousePixelX;
                SelectionStartY = MousePixelY;
                HasSelection = true;
            }
            if (HasSelection) {
                SelectionMinX = std::min(SelectionStartX, MousePixelX);
                SelectionMaxX = std::max(SelectionStartX, MousePixelX);
                SelectionMinY = std::min(SelectionStartY, MousePixelY);
                SelectionMaxY = std::max(SelectionStartY, MousePixelY);
            }
        } else {
            Layer* active = MyDocument->getActiveLayer();
            if (active) {
                if (StrokeStarted && CurrentTool != ToolMode::Picker) {
                    UndoLayerIndex = MyDocument->getActiveLayerIndex();
                    UndoBuffer = active->getBuffer();
                    UndoAvailable = true;
                    RedoAvailable = false;
                }
                if (CurrentTool == ToolMode::Brush) {
                    uint8_t r = NormalizeColor(BrushColor[0]);
                    uint8_t g = NormalizeColor(BrushColor[1]);
                    uint8_t b = NormalizeColor(BrushColor[2]);
                    active->DrawBrush(MousePixelX, MousePixelY, BrushSize, r, g, b, 255,
                        HasSelection, SelectionMinX, SelectionMinY, SelectionMaxX, SelectionMaxY);
                } else if (CurrentTool == ToolMode::Eraser) {
                    active->DrawBrush(MousePixelX, MousePixelY, BrushSize, 0, 0, 0, 0,
                        HasSelection, SelectionMinX, SelectionMinY, SelectionMaxX, SelectionMaxY);
                } else if (CurrentTool == ToolMode::Picker) {
                    std::vector<uint8_t> flat = MyDocument->Composite();
                    size_t idx = (static_cast<size_t>(MousePixelY) * MyDocument->getWidth() + MousePixelX) * 4;
                    BrushColor[0] = flat[idx + 0];
                    BrushColor[1] = flat[idx + 1];
                    BrushColor[2] = flat[idx + 2];
                }
            }
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z, false))
        Undo();
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y, false))
        Redo();

    DrawToolbar();
    DrawLayerPanel();

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

    MyCanvas->UpdateTexture(MyDocument->Composite());

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
    StrokeStarted = false;

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
    StrokeStarted = leftDown && !MouseDown;
    MouseDragging = leftDown && MouseDown;
    MouseDown = leftDown;
}

void UI::DrawLayerPanel() {
    ImGui::Separator();
    ImGui::Text("Calques");

    if (ImGui::Button("+ Ajouter")) {
        LayerCounter++;
        MyDocument->AddLayer("Layer " + std::to_string(LayerCounter));
    }
    ImGui::SameLine();
    if (ImGui::Button("- Supprimer")) {
        if (MyDocument->getLayerCount() > 1)
            MyDocument->RemoveLayer(MyDocument->getActiveLayerIndex());
    }

    for (int i = MyDocument->getLayerCount() - 1; i >= 0; i--) {
        Layer& layer = MyDocument->getLayer(i);
        ImGui::PushID(i);

        bool visible = layer.isVisible();
        if (ImGui::Checkbox("##visible", &visible))
            layer.setVisible(visible);

        ImGui::SameLine();

        bool isActive = (i == MyDocument->getActiveLayerIndex());
        if (ImGui::Selectable(layer.getName().c_str(), isActive))
            MyDocument->setActiveLayerIndex(i);

        ImGui::PopID();
    }
}

void UI::DrawToolbar() {
    ImGui::Separator();
    ImGui::Text("Outils");

    int toolIndex = (CurrentTool == ToolMode::Brush) ? 0 : (CurrentTool == ToolMode::Eraser) ? 1 : (CurrentTool == ToolMode::Picker) ? 2 : 3;
    if (ImGui::RadioButton("Pinceau", toolIndex == 0)) CurrentTool = ToolMode::Brush;
    ImGui::SameLine();
    if (ImGui::RadioButton("Gomme", toolIndex == 1)) CurrentTool = ToolMode::Eraser;
    ImGui::SameLine();
    if (ImGui::RadioButton("Pipette", toolIndex == 2)) CurrentTool = ToolMode::Picker;
    ImGui::SameLine();
    if (ImGui::RadioButton("Selection", toolIndex == 3)) CurrentTool = ToolMode::Selection;

    if (HasSelection) {
        ImGui::SameLine();
        if (ImGui::Button("Deselectionner"))
            HasSelection = false;
    }

    ImGui::SliderInt3("Couleur (RGB)", BrushColor, 0, 255, "%d", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SameLine();
    ImVec4 previewColor(BrushColor[0] / 255.0f, BrushColor[1] / 255.0f, BrushColor[2] / 255.0f, 1.0f);
    ImGui::ColorButton("Apercu couleur", previewColor, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(24, 24));

    ImGui::SliderInt("Taille pinceau", &BrushSize, 1, 50);

    ImGui::BeginDisabled(!UndoAvailable);
    if (ImGui::Button("Undo (Ctrl+Z)")) Undo();
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(!RedoAvailable);
    if (ImGui::Button("Redo (Ctrl+Y)")) Redo();
    ImGui::EndDisabled();
}

void UI::Undo() {
    if (!UndoAvailable)
        return;

    Layer& layer = MyDocument->getLayer(UndoLayerIndex);

    RedoLayerIndex = UndoLayerIndex;
    RedoBuffer = layer.getBuffer();
    RedoAvailable = true;

    layer.setBuffer(UndoBuffer);
    UndoAvailable = false;
}

void UI::Redo() {
    if (!RedoAvailable)
        return;

    Layer& layer = MyDocument->getLayer(RedoLayerIndex);

    UndoLayerIndex = RedoLayerIndex;
    UndoBuffer = layer.getBuffer();
    UndoAvailable = true;

    layer.setBuffer(RedoBuffer);
    RedoAvailable = false;
}

}
