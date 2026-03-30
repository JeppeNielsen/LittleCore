//
// Created by Jeppe Nielsen on 30/03/2026.
//

#include "CodeEditor.hpp"
#include "imgui.h"
#include <filesystem>

namespace {
    std::string ResolveWorkspaceRootFromSource() {
        std::filesystem::path path(__FILE__);
        for (int i = 0; i < 4; ++i) {
            path = path.parent_path();
        }
        return path.lexically_normal().generic_string();
    }

    std::string ResolveWorkspaceRoot(const std::string& projectRoot) {
        if (projectRoot.empty()) {
            return ResolveWorkspaceRootFromSource();
        }

        std::filesystem::path current = std::filesystem::path(projectRoot).lexically_normal();
        while (!current.empty()) {
            if (std::filesystem::exists(current / "Engine") && std::filesystem::exists(current / "Projects")) {
                return current.generic_string();
            }

            const auto parent = current.parent_path();
            if (parent == current) {
                break;
            }
            current = parent;
        }

        return ResolveWorkspaceRootFromSource();
    }

    std::string DefaultProjectRoot() {
        return ResolveWorkspaceRootFromSource() + "/Projects/CodeEditor";
    }
}

void CodeEditor::Initialize() {
    gui.Initialize(mainWindow, [this]() {
        DrawGui();
    });

    const auto projectRoot = context.projectRoot.empty()
                             ? DefaultProjectRoot()
                             : std::filesystem::path(context.projectRoot).lexically_normal().generic_string();

    workspaceRoot = ResolveWorkspaceRoot(projectRoot);
    projectWindow.SetRootPath(projectRoot);
    projectWindow.Refresh();

    autocomplete.SetWorkspaceRoot(workspaceRoot);
    SyncProjectFiles();
    LoadFonts();

    workspace.SetStatusText("Select a .cpp or .hpp file from the project tree.");
}

void CodeEditor::Update(float dt) {
    (void)dt;
}

void CodeEditor::Render() {
    gui.Render();
}

void CodeEditor::HandleEvent(void* event) {
    gui.HandleEvent(event);
}

void CodeEditor::LoadFonts() {
    const auto preferredFont = std::filesystem::path(workspaceRoot) / "External/imgui/misc/fonts/Cousine-Regular.ttf";
    if (std::filesystem::exists(preferredFont)) {
        codeFont = gui.LoadFont(preferredFont.generic_string(), 15);
        return;
    }

    const auto fallbackFont = context.engineRoot + "Fonts/LucidaG.ttf";
    if (std::filesystem::exists(fallbackFont)) {
        codeFont = gui.LoadFont(fallbackFont, 15);
    }
}

void CodeEditor::SyncProjectFiles() {
    autocomplete.SetProjectRoot(projectWindow.RootPath());
    autocomplete.SetCodeFiles(projectWindow.CodeFiles());
}

void CodeEditor::DrawGui() {
    ImGui::DockSpaceOverViewport();

    const auto result = projectWindow.Draw(workspace.ActivePath());
    if (result.refreshed) {
        SyncProjectFiles();
        workspace.SetStatusText("Refreshed project code files.");
    }

    if (!result.openedPath.empty()) {
        workspace.OpenFile(result.openedPath);
    }

    workspace.Draw(autocomplete, codeFont);
}
