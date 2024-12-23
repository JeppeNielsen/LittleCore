//
// Created by Jeppe Nielsen on 13/04/2024.
//
#include "Engine.hpp"
#include "ImGuiController.hpp"
#include "imgui.h"
#include "Vector2.hpp"

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
    
    int value;

    void Initialize() override {
        
        Vector2 v {2,3};
        
        v.y = 45;

        gui.Initialize(mainWindow, [this]() {
            OnGUI();
        });

        gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestImGui/Source/Fonts/LucidaG.ttf", 12);
   }

    void HandleEvent(void* event) override {
        gui.HandleEvent(event);
    }

    void OnGUI() {

        ImGui::DockSpaceOverViewport();

        ImGui::Begin("My test window");

        if (ImGui::Button("Show extra window")) {
            showWindow = !showWindow;
        }

        ImGui::End();

        ImGui::Begin("Window 2");

        ImGui::ColorEdit4("Color", &backgroundColor.r);

        ImGui::End();
        
        ImGui::Begin("Window 3");

        ImGui::ColorEdit4("Color", &backgroundColor.r);

        ImGui::End();
		
        if (showWindow) {
            ImGui::Begin("Extra window");
            
            ImGui::DragInt("Test int", &value);

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
    Engine e({"TestImGUI"});
    e.Start<ImguiTest>();
    return 0;
}
