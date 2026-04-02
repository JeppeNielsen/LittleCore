//
// Created by Jeppe Nielsen on 30/03/2026.
//

#include "CodeEditor.hpp"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <filesystem>

namespace {
    std::string EnsureTrailingSlash(const std::string& value) {
        if (value.empty() || value.back() == '/' || value.back() == '\\') {
            return value;
        }

        return value + "/";
    }

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

    std::string DefaultEngineAssetsPath(const std::string& workspaceRoot) {
        return workspaceRoot + "/Projects/TestNetimgui/Assets/";
    }

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
}

void CodeEditor::Initialize() {
    gui.Initialize(mainWindow, [this]() {
        DrawGui();
    });

    const auto projectRoot = context.projectRoot.empty()
                             ? DefaultProjectRoot()
                             : std::filesystem::path(context.projectRoot).lexically_normal().generic_string();
    const auto engineAssetsPath = context.engineRoot.empty()
                                  ? DefaultEngineAssetsPath(ResolveWorkspaceRoot(projectRoot))
                                  : EnsureTrailingSlash(std::filesystem::path(context.engineRoot).lexically_normal().generic_string());

    workspaceRoot = ResolveWorkspaceRoot(projectRoot);
    context.projectRoot = projectRoot;
    context.engineRoot = engineAssetsPath;

    autocomplete.SetWorkspaceRoot(workspaceRoot);

    auto& settings = targetProject.Settings();
    settings.workspaceRoot = workspaceRoot;
    settings.rootPath = projectRoot;
    settings.engineAssetsPath = engineAssetsPath;
    settings.cachePath = workspaceRoot + "/Projects/CodeEditor/Cache";

    ReloadTargetProject();
    LoadFonts();

    workspace.SetStatusText("Select a .cpp or .hpp file from the project tree.");
}

void CodeEditor::Update(float dt) {
    (void)dt;
    targetProject.Update();
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

void CodeEditor::ReloadTargetProject() {
    auto& settings = targetProject.Settings();
    settings.rootPath = std::filesystem::path(settings.rootPath).lexically_normal().generic_string();
    settings.engineAssetsPath = EnsureTrailingSlash(std::filesystem::path(settings.engineAssetsPath).lexically_normal().generic_string());
    settings.cachePath = std::filesystem::path(settings.cachePath).lexically_normal().generic_string();

    projectWindow.SetRootPath(settings.rootPath);
    projectWindow.Refresh();
    SyncProjectFiles();
    targetProject.Reload();
    targetProject.SetSourceBreakpoints(workspace.SourceBreakpoints());
}

void CodeEditor::DrawTargetProjectWindow() {
    auto& settings = targetProject.Settings();

    ImGui::Begin("Target Project");

    ImGui::InputText("Target Project Directory", &settings.rootPath);
    ImGui::InputText("Engine Assets Directory", &settings.engineAssetsPath);
    ImGui::InputText("Cache Directory", &settings.cachePath);

    if (ImGui::Button("Reload Target")) {
        ReloadTargetProject();
    }

    ImGui::SameLine();
    if (ImGui::Button("Compile All")) {
        for (auto& [id, program] : targetProject.GetPrograms()) {
            (void)id;
            program->StartBuild(false);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Compile All + Restart")) {
        for (auto& [id, program] : targetProject.GetPrograms()) {
            (void)id;
            program->StartBuild(true);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Stop All")) {
        for (auto& [id, program] : targetProject.GetPrograms()) {
            (void)id;
            program->StopProcess();
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

void CodeEditor::DrawProgramsWindow() {
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

            if (ImGui::Button("Open Source")) {
                workspace.OpenFile(program->Definition().SourcePath());
            }

            ImGui::SameLine();
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
                    if (ImGui::Button("Open Stop Location")) {
                        workspace.OpenFileAtLine(program->DebuggerLocationFile(), program->DebuggerLocationLine());
                    }
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

void CodeEditor::DrawGui() {
    ImGui::DockSpaceOverViewport();
    DrawTargetProjectWindow();

    const auto result = projectWindow.Draw(workspace.ActivePath());
    for (const auto& removedPath : result.removedPaths) {
        workspace.RemovePath(removedPath);
    }

    if (result.refreshed) {
        SyncProjectFiles();
        targetProject.Reload();
        targetProject.SetSourceBreakpoints(workspace.SourceBreakpoints());
        if (!result.statusText.empty()) {
            workspace.SetStatusText(result.statusText);
        } else {
            workspace.SetStatusText("Refreshed project code files.");
        }
    }

    if (!result.openedPath.empty()) {
        workspace.OpenFile(result.openedPath);
    } else if (!result.statusText.empty()) {
        workspace.SetStatusText(result.statusText);
    }

    DrawProgramsWindow();
    workspace.Draw(autocomplete, codeFont);
    const auto sourceBreakpoints = workspace.SourceBreakpoints();
    const auto breakpointResult = breakpointOverview.Draw(sourceBreakpoints, workspace.ActivePath());
    if (!breakpointResult.openedPath.empty()) {
        workspace.OpenFileAtLine(breakpointResult.openedPath, breakpointResult.openedLine);
    }

    if (workspace.ConsumeBreakpointsChanged()) {
        targetProject.SetSourceBreakpoints(sourceBreakpoints);
    }
}
