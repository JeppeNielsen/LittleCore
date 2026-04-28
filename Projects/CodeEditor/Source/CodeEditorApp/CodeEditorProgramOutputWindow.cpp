//
// Created by Codex on 04/04/2026.
//

#include "CodeEditorProgramOutputWindow.hpp"
#include "CodeEditorPathUtils.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

namespace {
    const std::string& CurrentProgramOutput(const Program& program) {
        return program.IsDebuggerActive() ? program.DebuggerConsoleOutput() : program.ProcessOutput();
    }

    const char* CurrentProgramOutputSource(const Program& program) {
        return program.IsDebuggerActive() ? "Debugger Console" : "Stdout / Stderr";
    }
}

void CodeEditorProgramOutputWindow::SetDisplayRootPath(std::string rootPath) {
    displayRootPath = std::move(rootPath);
}

void CodeEditorProgramOutputWindow::Draw(TargetProject::Programs& programs) {
    ImGui::Begin("Program Output");
    ImGui::Checkbox("Follow Output", &followOutput);

    if (programs.empty()) {
        ImGui::TextWrapped("Start or debug a discovered program to see its output here.");
        ImGui::End();
        return;
    }

    if (!ImGui::BeginTabBar("program-output-tabs")) {
        ImGui::End();
        return;
    }

    for (auto& [id, program] : programs) {
        if (!ImGui::BeginTabItem(id.c_str())) {
            continue;
        }

        ImGui::Text("Process State: %s", program->IsProcessRunning() ? "Running" : "Stopped");
        ImGui::Text("Output Source: %s", CurrentProgramOutputSource(*program));

        const auto displayExecutablePath = CodeEditorPathUtils::MakeDisplayPath(program->Definition().ExecutablePath(), displayRootPath);
        ImGui::TextWrapped("Executable: %s", displayExecutablePath.c_str());

        const auto& output = CurrentProgramOutput(*program);
        auto& outputBuffer = outputBuffers[id];
        if (outputBuffer != output) {
            outputBuffer = output;
        }

        ImGui::SameLine();
        if (ImGui::Button("Copy All")) {
            ImGui::SetClipboardText(output.c_str());
        }

        if (!program->IsDebuggerActive()) {
            ImGui::SameLine();
            if (ImGui::Button("Clear")) {
                program->ClearProcessOutput();
                lastOutputSizes[id] = 0;
                outputBuffer.clear();
            }
        }

        const bool outputChanged = lastOutputSizes[id] != output.size();
        const std::string outputLabel = "##program-output-text-" + id;
        const ImGuiID outputTextId = ImGui::GetID(outputLabel.c_str());

        if (output.empty()) {
            ImGui::TextDisabled("No output yet.");
        }

        ImGui::InputTextMultiline(
                outputLabel.c_str(),
                &outputBuffer,
                ImGui::GetContentRegionAvail(),
                ImGuiInputTextFlags_ReadOnly);

        if (followOutput && outputChanged) {
            if (ImGuiWindow* outputWindow = ImGui::FindWindowByID(outputTextId)) {
                ImGui::SetScrollY(outputWindow, outputWindow->ScrollMax.y);
            }
        }

        lastOutputSizes[id] = output.size();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::End();
}
