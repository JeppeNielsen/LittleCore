//
// Created by Jeppe Nielsen on 13/04/2024.
//
#include "Engine.hpp"
#include "ImGuiController.hpp"
#include "imgui.h"
#include "TextEditor.hpp"
#include "FileHelper.hpp"

using namespace LittleCore;

struct Color {
    float r,g,b,a;

    uint32_t GetUint() {
        uint8_t red = (255)*r;
        uint8_t green = (255)*g;
        uint8_t blue = (255)*b;
        uint8_t alpha = (255)*a;

        return red << 24 | green << 16 | blue << 8 | alpha;
    }

};
struct ImguiTest : IState {

    ImGuiController gui;

    Color backgroundColor;

    bool showWindow = false;

    TextEditor editor;
    ImFont* editorFont;

    void Initialize() override {

        gui.Initialize(mainWindow, [this]() {
            OnGUI();
        });

        gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestTextEditor/Source/Fonts/LucidaG.ttf", 12);
        editorFont = gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestTextEditor/Source/Fonts/JetBrainsMono-Regular.ttf", 16);

        editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());

        auto text = FileHelper::ReadAllText("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Scripts/ConsoleWindow/Console.cpp");
        editor.SetText(text);

        TextEditor::ErrorMarkers errorMarkers;
        errorMarkers[30] = "syntax error";

        editor.SetErrorMarkers(errorMarkers);

        editor.SetPalette(TextEditor::GetLightPalette());
    }

    void HandleEvent(void* event) override {
        gui.HandleEvent(event);
    }

    void OnGUI() {

        ImGui::DockSpaceOverViewport();

        ImGui::Begin("My test window");


        ImGui::PushFont(editorFont);
        editor.Render("Editor");
        ImGui::PopFont();

        ImGui::End();

        ImGui::Begin("Set errors");

        if (ImGui::Button("Show extra window")) {
            showWindow = !showWindow;

            TextEditor::ErrorMarkers errorMarkers;
            errorMarkers[50] = "Extra line";

            editor.SetErrorMarkers(errorMarkers);

            TextEditor::Breakpoints breakPoints;
            breakPoints.insert(30);
            breakPoints.insert(60);

            editor.SetBreakpoints(breakPoints);
        }

        ImGui::End();

        ImGui::Begin("Window 2");

        ImGui::ColorEdit4("Color", &backgroundColor.r);

        ImGui::End();

        if (showWindow) {
            ImGui::Begin("Extra window");

            ImGui::End();
        }

        ImGui::ShowDemoWindow();
    }

    void Update(float dt) override {

    }

    void Render() override {
        gui.Render();
    }

};

int main() {
    Engine e({"Test Text Editor"});
    e.Start<ImguiTest>();
    return 0;
}
