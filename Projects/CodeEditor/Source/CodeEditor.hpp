//
// Created by Jeppe Nielsen on 30/03/2026.
//

#pragma once
#include "IState.hpp"
#include "MainStateContext.hpp"
#include "ImGuiController.hpp"
#include "CodeEditorApp/CodeEditorProjectWindow.hpp"
#include "CodeEditorApp/CodeEditorAutocomplete.hpp"
#include "CodeEditorApp/CodeEditorWorkspace.hpp"
#include "../../EditorHub/Source/Project/TargetProject.hpp"
#include <string>

struct CodeEditor : public LittleCore::IState {
    LittleCore::MainStateContext context;

    void Initialize() override;
    void Update(float dt) override;
    void Render() override;
    void HandleEvent(void* event) override;

private:
    LittleCore::ImGuiController gui;
    CodeEditorProjectWindow projectWindow;
    CodeEditorAutocomplete autocomplete;
    CodeEditorWorkspace workspace;
    TargetProject targetProject;
    ImFont* codeFont = nullptr;
    std::string workspaceRoot;

    void DrawGui();
    void DrawTargetProjectWindow();
    void DrawProgramsWindow();
    void LoadFonts();
    void ReloadTargetProject();
    void SyncProjectFiles();
};
