//
// Created by Jeppe Nielsen on 13/04/2024.
//
#include "Engine.hpp"
#include "ImGuiController.hpp"
#include <iostream>
#include "NetimguiClient.hpp"
#include <thread>
#include <SDL3/SDL.h>

using namespace LittleCore;

struct TestNetimguiClient : IState {
    ImGuiController gui;
    NetimguiClient netimguiClient;
    
    void Initialize() override {
        gui.Initialize(mainWindow, [this]() {
            OnGUI();
        });

        gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestImGui/Source/Fonts/LucidaG.ttf", 12);

        netimguiClient.Start();
        netimguiClient.Connect("Test client", "localhost");

        while (netimguiClient.IsConnectionPending()) {
            std::this_thread::sleep_for(std::chrono::milliseconds (16));
        }
        if (!netimguiClient.IsConnected()) {
            std::cout << "couldn't connect\n";
        }



    }

    void HandleEvent(void* event) override {
        gui.HandleEvent(event);
    }

    void OnGUI() {
        ImGui::DockSpaceOverViewport();
        ImGui::Begin("My test window");

        if (ImGui::Button("Close")) {
            abort();
        }

        ImGui::End();
    }

    void Update(float dt) override {
    }

    void Render() override {
       gui.Render();
    }
};

int main() {
    Engine e({"Netimgui Client", false});
    e.Start<TestNetimguiClient>();
    return 0;
}

