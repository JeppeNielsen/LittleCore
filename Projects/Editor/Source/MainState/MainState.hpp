//
// Created by Jeppe Nielsen on 15/04/2024.
//


#pragma once
#include "IState.hpp"
#include "ImGuiController.hpp"
#include "../Modules/ModuleDefinitionsManager.hpp"
#include "../Modules/ModuleManager.hpp"
#include "../Project/ProjectSettings.hpp"
#include <fstream>
#include <ostream>
#include "TaskRunner.hpp"
#include "Timer.hpp"

struct MainState : LittleCore::IState {
    MainState();
    void Initialize() override;
    void Update(float dt) override;
    void Render() override;
    void HandleEvent(void* event) override;
    void Compile();
    LittleCore::ImGuiController gui;

    EngineContext engineContext;
    ModuleSettings moduleSettings;
    ModuleDefinitionsManager moduleDefinitionsManager;
    ModuleManager moduleManager;
    ProjectSettings projectSettings;
    RegistryCollection registyCollection;
    LittleCore::TaskRunner taskRunner;
    bool isCompiling = false;
    std::vector<std::string> errorsFromCompilation;
    LittleCore::Timer compilationTimer;

    std::ifstream cin;
    std::ofstream cout;
    std::vector<std::string> errors;
};

