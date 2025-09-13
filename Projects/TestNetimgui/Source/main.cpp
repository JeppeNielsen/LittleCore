//
// Created by Jeppe Nielsen on 13/04/2024.
//
#include "Engine.hpp"
#include "ImGuiController.hpp"
#include "NetimguiController.hpp"
#include <iostream>

using namespace LittleCore;

struct ImguiTest : IState {
    ImGuiController gui;
    NetimguiController netimguiController;

    ImguiTest() : netimguiController(gui) {}

    void Initialize() override {
        gui.Initialize(mainWindow, [this]() {
            OnGUI();
        });

        gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestImGui/Source/Fonts/LucidaG.ttf", 12);
        if (!netimguiController.Start()) {
            std::cout << "Netimgui lib failed\n";
        }
    }

    void HandleEvent(void* event) override {
        gui.HandleEvent(event);
    }

    void OnGUI() {
        ImGui::DockSpaceOverViewport();
        ImGui::Begin("My test window");
        ImGui::End();
        netimguiController.Draw();
    }

    void Update(float dt) override {

    }

    void Render() override {
        netimguiController.Update();
       gui.Render();
    }
};

int main() {
    Engine e({"Test Netimgui"});
    e.Start<ImguiTest>();
    return 0;
}

