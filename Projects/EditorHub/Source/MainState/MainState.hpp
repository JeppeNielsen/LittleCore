//
// Created by Jeppe Nielsen on 20/03/2026.
//

#pragma once
#include "IState.hpp"
#include "ImGuiController.hpp"
#include "../Project/TargetProject.hpp"

struct MainState : LittleCore::IState {
    void Initialize() override;
    void Update(float dt) override;
    void Render() override;
    void HandleEvent(void* event) override;

private:
    LittleCore::ImGuiController gui;
    TargetProject targetProject;

    void DrawGui();
    void DrawProjectWindow();
    void DrawProgramsWindow();
};
