//
// Created by Jeppe Nielsen on 15/04/2024.
//


#pragma once
#include <fstream>
#include <ostream>

#include "IState.hpp"
#include "ImGuiController.hpp"
#include "../Modules/ModuleDefinitionsManager.hpp"
#include "../Modules/ModuleManager.hpp"
#include "../Project/ProjectSettings.hpp"
#include "../Project/Project.hpp"
#include "../Compilation/ProjectCompiler.hpp"
#include "TaskRunner.hpp"
#include "Timer.hpp"
#include "../Windows/CompilerWindow.hpp"
#include "../Windows/HierarchyWindow.hpp"
#include "../Windows/InspectorWindow.hpp"
#include "../Windows/ProjectWindow.hpp"
#include "../Rendering/TextureRenderer.hpp"
#include "BgfxRenderer.hpp"
#include "DefaultResourceManager.hpp"

struct MainState : LittleCore::IState, public IProjectCompilerHandler {
    MainState();
    ~MainState();
    void Initialize() override;
    void Update(float dt) override;
    void Render() override;
    void HandleEvent(void* event) override;
    void CompilationFinished(CompilationResult& result) override;

    LittleCore::ImGuiController gui;

    EngineContext engineContext;
    Project project;
    ProjectCompiler projectCompiler;

    RegistryManager registryManager;
    CompilerWindow compilerWindow;
    HierarchyWindow hierarchyWindow;
    InspectorWindow inspectorWindow;
    ProjectWindow projectWindow;

    LittleCore::BGFXRenderer bgfxRenderer;
    TextureRenderer textureRenderer;
    EditorRenderer editorRenderer;
    ResourceLoader resourceLoader;
    ComponentDrawer componentDrawer;
    ComponentFactory componentFactory;

    std::ifstream cin;
    std::ofstream cout;

};

