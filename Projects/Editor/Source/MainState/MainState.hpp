//
// Created by Jeppe Nielsen on 15/04/2024.
//


#pragma once
#include "IState.hpp"
#include "ImGuiController.hpp"
#include "../Modules/ModuleDefinitionsManager.hpp"
#include "../Modules/ModuleManager.hpp"
#include <fstream>
#include <ostream>

struct MainState : LittleCore::IState {
    MainState();
    void Initialize() override;
    void Update(float dt) override;
    void Render() override;
    void HandleEvent(void* event) override;
    LittleCore::ImGuiController gui;

    EngineContext engineContext;
    ModuleSettings moduleSettings;
    ModuleDefinitionsManager moduleDefinitionsManager;
    ModuleManager moduleManager;

    std::ifstream cin;
    std::ofstream cout;
    std::string errors;
};

