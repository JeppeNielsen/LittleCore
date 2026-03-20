//
// Created by Jeppe Nielsen on 20/03/2026.
//

#include "MainState.hpp"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <filesystem>

namespace {
    const char* BuildStateText(const Program& program) {
        if (program.IsBuilding()) {
            return "Building";
        }

        if (!program.HasBuildResult()) {
            return "Not built";
        }

        return program.LastBuildResult().succeeded ? "Built" : "Build failed";
    }
}

void MainState::Initialize() {
    gui.Initialize(mainWindow, [this]() {
        DrawGui();
    });

    const auto fontPath = targetProject.Settings().engineAssetsPath + "Fonts/LucidaG.ttf";
    if (std::filesystem::exists(fontPath)) {
        gui.LoadFont(fontPath, 14);
    }

    targetProject.Reload();
}

void MainState::Update(float dt) {
    (void)dt;
    targetProject.Update();
}

void MainState::Render() {
    gui.Render();
}

void MainState::HandleEvent(void* event) {
    gui.HandleEvent(event);
}

void MainState::DrawGui() {
    ImGui::DockSpaceOverViewport();
    DrawProjectWindow();
    DrawProgramsWindow();
}

void MainState::DrawProjectWindow() {
    auto& settings = targetProject.Settings();

    ImGui::Begin("EditorHub");

    ImGui::InputText("Target Project Directory", &settings.rootPath);
    ImGui::InputText("Engine Assets Directory", &settings.engineAssetsPath);
    ImGui::InputText("Cache Directory", &settings.cachePath);

    if (ImGui::Button("Reload Target")) {
        targetProject.Reload();
    }

    ImGui::SameLine();
    if (ImGui::Button("Compile All")) {
        for (auto& program : targetProject.GetPrograms()) {
            program.second->StartBuild(false);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Compile All + Restart")) {
        for (auto& program : targetProject.GetPrograms()) {
            program.second->StartBuild(true);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Stop All")) {
        for (auto& program : targetProject.GetPrograms()) {
            program.second->StopProcess();
        }
    }

    ImGui::Separator();
    ImGui::Text("Discovered Programs: %zu", targetProject.GetPrograms().size());
    ImGui::TextWrapped("%s", targetProject.StatusText().c_str());

    if (!settings.HasValidRoot()) {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "The target project directory does not exist.");
    }

    if (!settings.HasValidEngineAssetsPath()) {
        ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.2f, 1.0f), "The engine assets directory does not exist. Child apps may start without the expected font assets.");
    }

    ImGui::End();
}

void MainState::DrawProgramsWindow() {
    ImGui::Begin("Programs");

    if (targetProject.GetPrograms().empty()) {
        ImGui::TextWrapped("Reload a target project directory containing child app state implementations to populate this list.");
        ImGui::End();
        return;
    }

    for (auto& [id, program] : targetProject.GetPrograms()) {
        ImGui::PushID(id.c_str());

        if (ImGui::CollapsingHeader(id.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("State Type: %s", program->Definition().StateTypeName().c_str());
            ImGui::Text("Build State: %s", BuildStateText(*program));
            ImGui::Text("Process State: %s", program->IsProcessRunning() ? "Running" : "Stopped");

            if (program->HasExitCode()) {
                ImGui::Text("Last Exit Code: %d", program->LastExitCode());
            }

            if (!program->RuntimeMessage().empty()) {
                ImGui::TextWrapped("Runtime: %s", program->RuntimeMessage().c_str());
            }

            ImGui::TextWrapped("Source: %s", program->Definition().SourcePath().c_str());
            ImGui::TextWrapped("Executable: %s", program->Definition().ExecutablePath().c_str());

            if (ImGui::Button("Compile")) {
                program->StartBuild(false);
            }

            ImGui::SameLine();
            if (ImGui::Button("Compile + Restart")) {
                program->StartBuild(true);
            }

            ImGui::SameLine();
            if (ImGui::Button("Start")) {
                program->StartProcess();
            }

            ImGui::SameLine();
            if (ImGui::Button("Stop")) {
                program->StopProcess();
            }

            ImGui::SameLine();
            if (ImGui::Button("Restart")) {
                program->RestartProcess();
            }

            if (program->HasBuildResult()) {
                ImGui::Separator();
                ImGui::TextWrapped("%s", program->LastBuildResult().summary.c_str());
                ImGui::Text("Last Build Duration: %.2f seconds", program->LastBuildDuration());

                if (ImGui::TreeNode("Compiler Command")) {
                    ImGui::TextWrapped("%s", program->LastBuildResult().command.c_str());
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Compiler Output")) {
                    ImGui::BeginChild("compiler-output", ImVec2(0.0f, 180.0f), true);
                    ImGui::TextUnformatted(program->LastBuildResult().output.c_str());
                    ImGui::EndChild();
                    ImGui::TreePop();
                }
            }
        }

        ImGui::PopID();
    }

    ImGui::End();
}
