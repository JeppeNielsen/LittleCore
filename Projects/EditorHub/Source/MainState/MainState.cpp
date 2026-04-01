//
// Created by Jeppe Nielsen on 20/03/2026.
//

#include "MainState.hpp"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <mach-o/dyld.h>
#include <spawn.h>
#include <unistd.h>
#include <vector>

extern char** environ;

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

    const char* DebuggerStateText(const Program& program) {
        if (!program.IsDebuggerActive()) {
            return "Inactive";
        }

        if (program.IsDebuggerRunning()) {
            return "Running";
        }

        if (program.IsDebuggerStopped()) {
            return "Paused";
        }

        return "Attached";
    }

    std::string BuildVariableLabel(const DebuggerVariable& variable) {
        std::string label = variable.name;
        if (!variable.type.empty()) {
            label += " : " + variable.type;
        }

        if (!variable.value.empty()) {
            label += " = " + variable.value;
        }

        return label;
    }

    void DrawDebuggerVariable(Program& program, const DebuggerVariable& variable, int index) {
        const auto label = BuildVariableLabel(variable);
        if (variable.variablesReference == 0) {
            ImGui::PushID(index);
            ImGui::Bullet();
            ImGui::SameLine();
            if (variable.variablesLoading && !variable.variablesLoaded) {
                ImGui::TextDisabled("%s (Loading...)", variable.name.c_str());
            } else {
                ImGui::TextWrapped("%s", label.c_str());
            }
            ImGui::PopID();
            return;
        }

        ImGui::PushID(index);
        const bool showAsLeaf = variable.variablesLoaded && variable.variables.empty();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
        if (showAsLeaf) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        const bool isOpen = ImGui::TreeNodeEx("variable", flags, "%s", label.c_str());
        if (isOpen && !showAsLeaf) {
            program.EnsureDebuggerVariableChildrenLoaded(variable.variablesReference);
            if (variable.variablesLoading || !variable.variablesLoaded) {
                ImGui::TextDisabled("Loading...");
            } else if (variable.variables.empty()) {
                ImGui::TextDisabled("No child variables.");
            } else {
                for (std::size_t childIndex = 0; childIndex < variable.variables.size(); ++childIndex) {
                    DrawDebuggerVariable(program, variable.variables[childIndex], static_cast<int>(childIndex));
                }
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void DrawDebuggerScopes(Program& program) {
        if (!program.IsDebuggerStopped()) {
            return;
        }

        if (!ImGui::TreeNode("Variables")) {
            return;
        }

        const auto& scopes = program.DebuggerScopes();
        if (scopes.empty()) {
            ImGui::TextDisabled("Loading current frame variables...");
            ImGui::TreePop();
            return;
        }

        bool drewScope = false;
        for (const auto& scope : scopes) {
            drewScope = true;
            if (ImGui::TreeNode(scope.name.c_str())) {
                if (scope.variablesLoading || !scope.variablesLoaded) {
                    ImGui::TextDisabled("Loading...");
                } else if (scope.variables.empty()) {
                    ImGui::TextDisabled("No variables.");
                } else {
                    for (std::size_t variableIndex = 0; variableIndex < scope.variables.size(); ++variableIndex) {
                        DrawDebuggerVariable(program, scope.variables[variableIndex], static_cast<int>(variableIndex));
                    }
                }
                ImGui::TreePop();
            }
        }

        if (!drewScope) {
            ImGui::TextDisabled("No variables available for the current frame.");
        }

        ImGui::TreePop();
    }

    bool IsExecutableFile(const std::filesystem::path& path) {
        return std::filesystem::exists(path) &&
               std::filesystem::is_regular_file(path) &&
               access(path.c_str(), X_OK) == 0;
    }

    void AddCandidate(std::vector<std::filesystem::path>& candidates, const std::filesystem::path& path) {
        if (!path.empty()) {
            candidates.push_back(path.lexically_normal());
        }
    }

    std::string CurrentExecutablePath() {
        std::vector<char> buffer(1024);
        uint32_t size = static_cast<uint32_t>(buffer.size());
        if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
            buffer.resize(size);
            if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
                return {};
            }
        }

        return std::filesystem::path(buffer.data()).lexically_normal().generic_string();
    }

    bool ShouldSkipSearchPath(const std::filesystem::path& rootPath, const std::filesystem::path& path) {
        std::error_code errorCode;
        const auto relativePath = std::filesystem::relative(path, rootPath, errorCode);
        if (errorCode) {
            return false;
        }

        for (const auto& part : relativePath) {
            const auto name = part.string();
            if (name == ".git" || name == "External" || name == "Cache") {
                return true;
            }

            if (!name.empty() && name[0] == '.') {
                return true;
            }
        }

        return false;
    }

    std::string FindCodeEditorExecutable(const TargetProjectSettings& settings) {
        std::vector<std::filesystem::path> candidates;
        const auto currentExecutable = CurrentExecutablePath();
        if (!currentExecutable.empty()) {
            const auto currentPath = std::filesystem::path(currentExecutable);
            AddCandidate(candidates, currentPath.parent_path() / "CodeEditor");
            AddCandidate(candidates, currentPath.parent_path().parent_path().parent_path().parent_path() / "CodeEditor.app/Contents/MacOS/CodeEditor");
        }

        const auto workspaceRoot = std::filesystem::path(settings.workspaceRoot);
        AddCandidate(candidates, workspaceRoot / "bin/Debug/CodeEditor");
        AddCandidate(candidates, workspaceRoot / "bin/Release/CodeEditor");
        AddCandidate(candidates, workspaceRoot / "Projects/CodeEditor/Build/bin/Debug/CodeEditor");
        AddCandidate(candidates, workspaceRoot / "Projects/CodeEditor/Build/bin/Release/CodeEditor");
        AddCandidate(candidates, workspaceRoot / "Projects/CodeEditor/Build/bin/ARM64/Debug/CodeEditor");
        AddCandidate(candidates, workspaceRoot / "Projects/CodeEditor/Build/bin/ARM64/Release/CodeEditor");
        AddCandidate(candidates, workspaceRoot / "Projects/CodeEditor/Build/bin/Debug/CodeEditor.app/Contents/MacOS/CodeEditor");
        AddCandidate(candidates, workspaceRoot / "Projects/CodeEditor/Build/bin/Release/CodeEditor.app/Contents/MacOS/CodeEditor");
        AddCandidate(candidates, workspaceRoot / "Projects/CodeEditor/Build/bin/ARM64/Debug/CodeEditor.app/Contents/MacOS/CodeEditor");
        AddCandidate(candidates, workspaceRoot / "Projects/CodeEditor/Build/bin/ARM64/Release/CodeEditor.app/Contents/MacOS/CodeEditor");

        for (const auto& candidate : candidates) {
            if (IsExecutableFile(candidate)) {
                return candidate.generic_string();
            }
        }

        std::error_code errorCode;
        for (std::filesystem::recursive_directory_iterator iterator(workspaceRoot, errorCode), end; iterator != end; iterator.increment(errorCode)) {
            if (errorCode) {
                errorCode.clear();
                continue;
            }

            const auto& entry = *iterator;
            if (ShouldSkipSearchPath(workspaceRoot, entry.path())) {
                if (entry.is_directory()) {
                    iterator.disable_recursion_pending();
                }
                continue;
            }

            if (entry.is_regular_file() && entry.path().filename() == "CodeEditor" && IsExecutableFile(entry.path())) {
                return entry.path().lexically_normal().generic_string();
            }

            if (entry.is_directory() && entry.path().filename() == "CodeEditor.app") {
                const auto bundledExecutable = entry.path() / "Contents/MacOS/CodeEditor";
                if (IsExecutableFile(bundledExecutable)) {
                    return bundledExecutable.lexically_normal().generic_string();
                }
            }
        }

        return {};
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
    if (ImGui::Button("Open CodeEditor")) {
        LaunchCodeEditor();
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
    if (!codeEditorStatusText.empty()) {
        ImGui::TextWrapped("%s", codeEditorStatusText.c_str());
    }

    if (!settings.HasValidRoot()) {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "The target project directory does not exist.");
    }

    if (!settings.HasValidEngineAssetsPath()) {
        ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.2f, 1.0f), "The engine assets directory does not exist. Child apps may start without the expected font assets.");
    }

    ImGui::End();
}

void MainState::LaunchCodeEditor() {
    auto& settings = targetProject.Settings();
    if (settings.rootPath.empty()) {
        codeEditorStatusText = "Set a target project directory before opening CodeEditor.";
        return;
    }

    const auto executablePath = FindCodeEditorExecutable(settings);
    if (executablePath.empty()) {
        codeEditorStatusText = "Could not find a built CodeEditor executable in the workspace.";
        return;
    }

    std::vector<std::string> arguments = {
            executablePath,
            "--project-root",
            settings.rootPath,
            "--engine-root",
            settings.engineAssetsPath
    };
    std::vector<char*> argv;
    argv.reserve(arguments.size() + 1);
    for (auto& argument : arguments) {
        argv.push_back(const_cast<char*>(argument.c_str()));
    }
    argv.push_back(nullptr);

    pid_t pid = 0;
    const int result = posix_spawn(&pid, executablePath.c_str(), nullptr, nullptr, argv.data(), environ);
    if (result != 0) {
        codeEditorStatusText = "Failed to launch CodeEditor: " + std::string(std::strerror(result));
        return;
    }

    codeEditorStatusText = "Opened CodeEditor for " + settings.rootPath;
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
            ImGui::Text("Debugger State: %s", DebuggerStateText(*program));

            if (program->HasExitCode()) {
                ImGui::Text("Last Exit Code: %d", program->LastExitCode());
            }

            if (!program->RuntimeMessage().empty()) {
                ImGui::TextWrapped("Runtime: %s", program->RuntimeMessage().c_str());
            }

            if (program->IsDebuggerActive() && !program->DebuggerStatusText().empty()) {
                ImGui::TextWrapped("Debugger: %s", program->DebuggerStatusText().c_str());
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
            if (ImGui::Button("Debug")) {
                program->StartDebugging();
            }

            ImGui::SameLine();
            if (ImGui::Button("Attach LLDB")) {
                program->AttachDebugger();
            }

            ImGui::SameLine();
            if (ImGui::Button("Stop")) {
                program->StopProcess();
            }

            ImGui::SameLine();
            if (ImGui::Button("Restart")) {
                program->RestartProcess();
            }

            if (program->IsDebuggerActive()) {
                if (ImGui::Button("Continue")) {
                    program->ContinueDebugger();
                }

                ImGui::SameLine();
                if (ImGui::Button("Pause")) {
                    program->PauseDebugger();
                }

                ImGui::SameLine();
                if (ImGui::Button("Step Into")) {
                    program->StepIntoDebugger();
                }

                ImGui::SameLine();
                if (ImGui::Button("Step Over")) {
                    program->StepOverDebugger();
                }

                ImGui::SameLine();
                if (ImGui::Button("Step Out")) {
                    program->StepOutDebugger();
                }

                if (program->IsDebuggerAttachedToProcess()) {
                    ImGui::SameLine();
                    if (ImGui::Button("Detach")) {
                        program->DetachDebugger();
                    }
                }

                if (program->HasDebuggerLocation()) {
                    ImGui::TextWrapped("Paused At: %s:%d",
                                       program->DebuggerLocationFile().c_str(),
                                       program->DebuggerLocationLine());
                }

                DrawDebuggerScopes(*program);

                if (!program->DebuggerConsoleOutput().empty() && ImGui::TreeNode("Debugger Output")) {
                    ImGui::BeginChild("debugger-output", ImVec2(0.0f, 180.0f), true);
                    ImGui::TextUnformatted(program->DebuggerConsoleOutput().c_str());
                    ImGui::EndChild();
                    ImGui::TreePop();
                }
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
