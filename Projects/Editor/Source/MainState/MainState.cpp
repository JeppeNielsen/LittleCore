//
// Created by Jeppe Nielsen on 15/04/2024.
//

#include "MainState.hpp"
#include "imgui.h"
#include <string>
#include "misc/cpp/imgui_stdlib.h"
#include <iostream>
#include <fstream>
#include "Timer.hpp"

MainState::MainState() :
project(engineContext),
cin("input.txt"),
cout("output.txt"),
projectCompiler(project, *this),
compilerWindow(projectCompiler)
{

}

MainState::~MainState() {

}

void MainState::Initialize() {

    engineContext.registryCollection = &registyCollection;

    // optional performance optimizations
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    std::cin.rdbuf(cin.rdbuf());
    //std::cout.rdbuf(cout.rdbuf());

    project.LoadProject("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/TestProject");

    gui.Initialize(mainWindow, [this]() {
        ImGui::DockSpaceOverViewport();

        compilerWindow.DrawGui();

        for(auto& module : project.moduleManager.GetModules()) {
            module.second->OnGui(engineContext.imGuiContext);
        }
    });

    gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Assets/Fonts/LucidaG.ttf", 14);
    engineContext.imGuiContext = ImGui::GetCurrentContext();

}

void MainState::Update(float dt) {
    projectCompiler.Update();

    for(auto& module : project.moduleManager.GetModules()) {
        module.second->Update(dt);
    }
}

void MainState::Render() {
    ImGui::SetCurrentContext(engineContext.imGuiContext);
    gui.Render();

    for(auto& module : project.moduleManager.GetModules()) {
        module.second->Render();
    }
}

void MainState::HandleEvent(void *event) {
    gui.HandleEvent(event);
}

void MainState::CompilationFinished(CompilationResult &result) {
    engineContext.registryCollection->registries.clear();
}
