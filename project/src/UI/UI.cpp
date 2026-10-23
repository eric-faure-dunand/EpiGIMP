#include "UI.hpp"

#include <algorithm>
#include <cmath>

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
    Theme::Apply();

    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Icons = std::make_unique<IconLibrary>(EPIGIMP_ASSETS_DIR "/icons");
    LoadIcons();

    MyDocument = std::make_unique<Document>(1, 1);
    MyDocument->LoadBaseLayerFromFile(imagePath);

    MyCanvas = std::make_unique<Canvas>(MyDocument->getWidth(), MyDocument->getHeight());
    MyCanvas->UpdateTexture(MyDocument->Composite());
}

UI::~UI() {
    MyCanvas.reset();
    Icons.reset();

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

float UI::ToolbarRowHeight() const {
    return ToolIconSize + ImGui::GetStyle().FramePadding.y * 2.0f;
}

void UI::LoadIcons() {
    for (const ToolInfo& tool : GetTools())
        Icons->Load(tool.IconName, std::string(tool.IconName) + ".png");
    Icons->Load("mask", "mask.png");
    Icons->Load("deselect", "deselect.png");
    Icons->Load("undo", "undo.png");
    Icons->Load("redo", "redo.png");
}

void UI::HandleShortcuts() {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantTextInput)
        return;

    for (const ToolInfo& tool : GetTools()) {
        if (ImGui::IsKeyChordPressed(tool.Shortcut))
            CurrentTool = tool.Mode;
    }

    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Z))
        Undo();
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Y))
        Redo();
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_A))
        HasSelection = false;

    if (io.KeyMods == ImGuiMod_None) {
        if (ImGui::IsKeyPressed(ImGuiKey_LeftBracket) || ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract))
            BrushSize = std::max(BrushSizeMin, BrushSize - 1);
        if (ImGui::IsKeyPressed(ImGuiKey_RightBracket) || ImGui::IsKeyPressed(ImGuiKey_KeypadAdd))
            BrushSize = std::min(BrushSizeMax, BrushSize + 1);
    }
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

    DrawMenuBar();

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImGuiStyle& style = ImGui::GetStyle();
    const float toolbarHeight = ToolbarRowHeight() + ImGui::GetTextLineHeight()
                              + style.ItemSpacing.y + style.WindowPadding.y * 2.0f;
    const float statusBarHeight = ImGui::GetTextLineHeight() + StatusBarPadding.y * 2.0f;
    const ImVec2 origin = viewport->WorkPos;
    const ImVec2 size = viewport->WorkSize;

    ImGui::SetNextWindowPos(origin);
    ImGui::SetNextWindowSize(ImVec2(size.x, toolbarHeight));
    ImGui::Begin("Toolbar", nullptr, PanelFlags);
    DrawToolbar();
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(origin.x, origin.y + toolbarHeight));
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y - toolbarHeight - statusBarHeight));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("Workspace", nullptr, PanelFlags);
    ImGui::PopStyleVar(2);
    DrawWorkspace();
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(origin.x, origin.y + size.y - statusBarHeight));
    ImGui::SetNextWindowSize(ImVec2(size.x, statusBarHeight));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, StatusBarPadding);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, Theme::BarBg);
    ImGui::Begin("StatusBar", nullptr, PanelFlags);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    DrawStatusBar();
    ImGui::End();

    HandleShortcuts();

    MyCanvas->UpdateTexture(MyDocument->Composite());

    ImGui::Render();

    int w, h;
    glfwGetFramebufferSize(Window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(Theme::CanvasBg.x, Theme::CanvasBg.y, Theme::CanvasBg.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(Window);
}

void UI::DrawWorkspace() {
    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const float maxPanelWidth = std::max(SidePanelMinWidth, avail.x - SplitterWidth - CanvasMinWidth);
    SidePanelWidth = std::clamp(SidePanelWidth, SidePanelMinWidth, maxPanelWidth);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::CanvasBg);
    ImGui::BeginChild("CanvasArea", ImVec2(avail.x - SidePanelWidth - SplitterWidth, avail.y),
        ImGuiChildFlags_AlwaysUseWindowPadding);
    ImGui::PopStyleColor();
    DrawCanvas();
    ImGui::EndChild();

    ImGui::SameLine(0.0f, 0.0f);
    SidePanelWidth -= Widgets::VerticalSplitter("##side_panel_splitter", SplitterWidth, avail.y);

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginChild("SidePanel", ImVec2(0.0f, avail.y), ImGuiChildFlags_AlwaysUseWindowPadding);
    DrawLayerPanel();
    ImGui::Spacing();
    DrawExportPanel();
    ImGui::EndChild();
}

void UI::DrawCanvas() {
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
                    SaveUndoSnapshot();
                    UndoLayerIndex = MyDocument->getActiveLayerIndex();
                    UndoBuffer = active->getBuffer();
                    UndoAvailable = true;
                    RedoAvailable = false;
                }
                if (EditingMask) {
                    if (CurrentTool == ToolMode::Brush) {
                        active->DrawMaskBrush(MousePixelX, MousePixelY, BrushSize, 255,
                            HasSelection, SelectionMinX, SelectionMinY, SelectionMaxX, SelectionMaxY);
                    } else if (CurrentTool == ToolMode::Eraser) {
                        active->DrawMaskBrush(MousePixelX, MousePixelY, BrushSize, 0,
                            HasSelection, SelectionMinX, SelectionMinY, SelectionMaxX, SelectionMaxY);
                    }
                } else {
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
    }
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
    ImGui::SeparatorText("Calques");

    const float buttonWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
    if (ImGui::Button("+ Ajouter", ImVec2(buttonWidth, 0.0f))) {
        LayerCounter++;
        MyDocument->AddLayer("Layer " + std::to_string(LayerCounter));
    }
    ImGui::SameLine();
    if (ImGui::Button("- Supprimer", ImVec2(buttonWidth, 0.0f))) {
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

void UI::DrawExportPanel() {
    ImGui::SeparatorText("Export");

    ImGui::TextDisabled("Export path");
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputText("##export_path", ExportPathBuffer, sizeof(ExportPathBuffer));
    if (ImGui::Button("Export", ImVec2(-FLT_MIN, 0.0f))) {
        try {
            MyDocument->SaveToFile(ExportPathBuffer);
            StatusMessage = "Exporte: " + std::string(ExportPathBuffer);
        } catch (const IError& e) {
            StatusMessage = std::string("Erreur: ") + e.what();
        }
    }
}

void UI::DrawToolbar() {
    const ToolInfo& current = GetToolInfo(CurrentTool);
    const ImGuiStyle& style = ImGui::GetStyle();
    const float rowHeight = ToolbarRowHeight();
    const float groupHeight = rowHeight + style.ItemSpacing.y + ImGui::GetTextLineHeight();
    const float frameOffset = std::floor((rowHeight - ImGui::GetFrameHeight()) * 0.5f);

    Widgets::BeginRibbonGroup();
    bool first = true;
    for (const ToolInfo& tool : GetTools()) {
        if (!first)
            ImGui::SameLine();
        first = false;
        if (Widgets::IconButton(tool.IconName, Icons->Get(tool.IconName), tool.Label, ToolIconSize, tool.Mode == CurrentTool))
            CurrentTool = tool.Mode;
        Widgets::RichTooltip(tool.Label, tool.ShortcutLabel, tool.Description);
    }
    Widgets::EndRibbonGroup("Outils", rowHeight);

    Widgets::VerticalSeparator(groupHeight);
    Widgets::BeginRibbonGroup();
    if (Widgets::IconButton("mask", Icons->Get("mask"), "Masque", ToolIconSize, EditingMask))
        EditingMask = !EditingMask;
    Widgets::RichTooltip(EditingMask ? "Edition du masque : activee" : "Edition du masque : desactivee", nullptr,
        "Le pinceau et la gomme modifient le masque du calque actif au lieu de ses pixels.");

    ImGui::SameLine();
    ImGui::BeginDisabled(!HasSelection);
    if (Widgets::IconButton("deselect", Icons->Get("deselect"), "Deselectionner", ToolIconSize))
        HasSelection = false;
    ImGui::EndDisabled();
    Widgets::RichTooltip("Deselectionner", "Ctrl+Shift+A",
        HasSelection ? "Supprime la selection : les outils agissent de nouveau sur tout le calque."
                     : "Aucune selection active.");
    Widgets::EndRibbonGroup("Masque", rowHeight);

    Widgets::VerticalSeparator(groupHeight);
    Widgets::BeginRibbonGroup();
    ImGui::BeginDisabled(!UndoAvailable);
    if (Widgets::IconButton("undo", Icons->Get("undo"), "Annuler", ToolIconSize))
        Undo();
    ImGui::EndDisabled();
    Widgets::RichTooltip("Annuler", "Ctrl+Z", "Annule la derniere modification.");

    ImGui::SameLine();
    ImGui::BeginDisabled(!RedoAvailable);
    if (Widgets::IconButton("redo", Icons->Get("redo"), "Retablir", ToolIconSize))
        Redo();
    ImGui::EndDisabled();
    Widgets::RichTooltip("Retablir", "Ctrl+Y", "Retablit la derniere modification annulee.");
    Widgets::EndRibbonGroup("Historique", rowHeight);

    Widgets::VerticalSeparator(groupHeight);
    Widgets::BeginRibbonGroup(frameOffset);
    float color[3] = {BrushColor[0] / 255.0f, BrushColor[1] / 255.0f, BrushColor[2] / 255.0f};
    ImGui::SetNextItemWidth(ColorFieldWidth);
    if (ImGui::ColorEdit3("##couleur", color, ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB)) {
        for (int i = 0; i < 3; i++)
            BrushColor[i] = NormalizeColor(static_cast<int>(color[i] * 255.0f + 0.5f));
    }
    Widgets::EndRibbonGroup("Couleur", rowHeight);

    Widgets::VerticalSeparator(groupHeight);
    Widgets::BeginRibbonGroup();
    const float swatchSpacing = style.ItemSpacing.y;
    const float swatchSize = std::floor((rowHeight - swatchSpacing) * 0.5f);
    BasePalette.Draw("base_palette", swatchSize, swatchSpacing, BrushColor);
    Widgets::EndRibbonGroup("Palette", rowHeight);

    Widgets::VerticalSeparator(groupHeight);
    Widgets::BeginRibbonGroup(frameOffset);
    ImGui::BeginDisabled(!current.UsesSize);
    ImGui::SetNextItemWidth(SizeSliderWidth);
    ImGui::SliderInt("##taille", &BrushSize, BrushSizeMin, BrushSizeMax, "%d px", ImGuiSliderFlags_AlwaysClamp);
    ImGui::EndDisabled();
    Widgets::RichTooltip("Taille du pinceau", "[ / ]",
        current.UsesSize ? "Rayon du pinceau et de la gomme, en pixels. Aussi : - / + du pave numerique."
                         : "Sans effet sur l'outil actif.");
    Widgets::EndRibbonGroup("Taille", rowHeight);
}

void UI::DrawStatusBar() {
    const ToolInfo& current = GetToolInfo(CurrentTool);

    ImGui::Text("%s [%s]", current.Label, current.ShortcutLabel);
    if (EditingMask) {
        ImGui::SameLine();
        ImGui::TextColored(Theme::MaskWarning, "- mode masque");
    }
    if (HasSelection) {
        ImGui::SameLine();
        ImGui::TextDisabled("- selection %dx%d px",
            SelectionMaxX - SelectionMinX + 1, SelectionMaxY - SelectionMinY + 1);
    }
    if (!StatusMessage.empty()) {
        Widgets::VerticalSeparator(ImGui::GetTextLineHeight());
        ImGui::TextUnformatted(StatusMessage.c_str());
    }
}

void UI::DrawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Filtres")) {
            Layer* active = MyDocument->getActiveLayer();

            if (ImGui::MenuItem("Niveaux de gris") && active) {
                SaveUndoSnapshot();
                Filter::Grayscale(*active);
                StatusMessage = "Niveaux de gris applique sur: " + active->getName();
            }
            if (ImGui::MenuItem("Inversion") && active) {
                SaveUndoSnapshot();
                Filter::Invert(*active);
                StatusMessage = "Inversion appliquee sur: " + active->getName();
            }
            if (ImGui::MenuItem("Luminosite / Contraste..."))
                OpenBrightnessPopup = true;

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (OpenBrightnessPopup) {
        ImGui::OpenPopup("Luminosite / Contraste");
        OpenBrightnessPopup = false;
    }

    if (ImGui::BeginPopupModal("Luminosite / Contraste", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SliderInt("Luminosite", &FilterBrightness, -100, 100);
        ImGui::SliderInt("Contraste", &FilterContrast, -100, 100);

        if (ImGui::Button("Appliquer")) {
            Layer* active = MyDocument->getActiveLayer();
            if (active) {
                SaveUndoSnapshot();
                Filter::BrightnessContrast(*active, FilterBrightness, FilterContrast);
                StatusMessage = "Luminosite/contraste applique sur: " + active->getName();
            }
            FilterBrightness = 0;
            FilterContrast = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Annuler")) {
            FilterBrightness = 0;
            FilterContrast = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void UI::Undo() {
    if (!UndoAvailable)
        return;

    Layer& layer = MyDocument->getLayer(UndoLayerIndex);

    RedoLayerIndex = UndoLayerIndex;
    RedoBuffer = layer.getBuffer();
    RedoMask = layer.getMask();
    RedoAvailable = true;

    layer.setBuffer(UndoBuffer);
    layer.setMask(UndoMask);
    UndoAvailable = false;
}

void UI::Redo() {
    if (!RedoAvailable)
        return;

    Layer& layer = MyDocument->getLayer(RedoLayerIndex);

    UndoLayerIndex = RedoLayerIndex;
    UndoBuffer = layer.getBuffer();
    UndoMask = layer.getMask();
    UndoAvailable = true;

    layer.setBuffer(RedoBuffer);
    layer.setMask(RedoMask);
    RedoAvailable = false;
}

void UI::SaveUndoSnapshot() {
    Layer* active = MyDocument->getActiveLayer();
    if (!active)
        return;

    UndoLayerIndex = MyDocument->getActiveLayerIndex();
    UndoBuffer = active->getBuffer();
    UndoMask = active->getMask();
    UndoAvailable = true;
    RedoAvailable = false;
}

}
