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
        compilerWindow(projectCompiler),
        hierarchyWindow(registryManager),
        inspectorWindow(registryManager, componentDrawer, componentFactory),
        projectWindow(project),
        textureRenderer(bgfxRenderer),
        editorRenderer([this](const std::string& id, int width, int height, EditorRenderer::Callback callback) {
            textureRenderer.Render(id, width, height, callback);
        }, [this] (const std::string& id)  {
            return textureRenderer.GetTexture(id);
        }),
        resourceLoader([this](const std::string& id) {
            return project.defaultResourceManager.Create<LittleCore::ShaderResource>(id);
        },
        [this](const std::string& id) {
            return project.defaultResourceManager.Create<LittleCore::TextureResource>(id);
        })
{

}

MainState::~MainState() {

}

void MainState::Initialize() {

    engineContext.registryManager = &registryManager;

    // optional performance optimizations
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    std::cin.rdbuf(cin.rdbuf());
    //std::cout.rdbuf(cout.rdbuf());

    gui.Initialize(mainWindow, [this]() {
        ImGui::DockSpaceOverViewport();

        compilerWindow.DrawGui();
        hierarchyWindow.DrawGui();
        inspectorWindow.DrawGui();
        projectWindow.DrawGui();

        for(auto& module : project.moduleManager.GetModules()) {
            module.second->OnGui(engineContext.imGuiContext);
        }
    });

    gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/Assets/Fonts/LucidaG.ttf", 14);
    engineContext.imGuiContext = ImGui::GetCurrentContext();
    engineContext.editorRenderer = &editorRenderer;
    engineContext.resourceLoader = &resourceLoader;
    engineContext.componentDrawer = &componentDrawer;
    engineContext.componentFactory = &componentFactory;

    project.LoadProject("/Users/jeppe/Jeppes/LittleCore/Projects/Editor/TestProject/Assets");
}

void MainState::Update(float dt) {
    projectCompiler.Update();

    for(auto& module : project.moduleManager.GetModules()) {
        module.second->Update(dt);
    }
}

void MainState::Render() {
    for(auto& module : project.moduleManager.GetModules()) {
        module.second->Render(&editorRenderer);
    }

    ImGui::SetCurrentContext(engineContext.imGuiContext);
    gui.Render();
}

void MainState::HandleEvent(void *event) {
    gui.HandleEvent(event);
}

void MainState::CompilationFinished(CompilationResult &result) {
    registryManager.Clear();
    componentDrawer.Clear();
    componentFactory.Clear();
}
