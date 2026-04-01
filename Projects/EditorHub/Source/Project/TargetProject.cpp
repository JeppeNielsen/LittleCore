//
// Created by Jeppe Nielsen on 20/03/2026.
//

#include "TargetProject.hpp"
#include "../Programs/ProgramCompilerContext.hpp"
#include <filesystem>
#include <vector>

std::string TargetProject::ResolveWorkspaceRoot() {
    std::filesystem::path path(__FILE__);
    for (int i = 0; i < 5; ++i) {
        path = path.parent_path();
    }
    return path.lexically_normal().generic_string();
}

TargetProject::TargetProject() {
    settings.workspaceRoot = ResolveWorkspaceRoot();
    settings.rootPath = settings.workspaceRoot + "/Projects/TestNetimguiClient/Source/Assets";
    settings.engineAssetsPath = settings.workspaceRoot + "/Projects/TestNetimgui/Assets/";
    settings.cachePath = settings.workspaceRoot + "/Projects/EditorHub/Cache";
}

TargetProject::~TargetProject() = default;

TargetProjectSettings& TargetProject::Settings() {
    return settings;
}

const TargetProjectSettings& TargetProject::Settings() const {
    return settings;
}

void TargetProject::Reload() {
    programs.clear();

    if (!settings.HasValidRoot()) {
        statusText = "Target project directory was not found.";
        return;
    }

    ProgramCompilerContext sharedContext;
    std::vector<DiscoveredProgram> discoveredPrograms;
    settings.Populate(sharedContext, discoveredPrograms);

    std::filesystem::create_directories(settings.cachePath);

    for (const auto& discoveredProgram : discoveredPrograms) {
        ProgramDefinition definition(
                settings.rootPath,
                settings.engineAssetsPath,
                settings.cachePath,
                sharedContext,
                discoveredProgram);

        programs.insert({
                discoveredProgram.id,
                std::make_unique<Program>(std::move(definition), settings.rootPath)
        });
        programs.at(discoveredProgram.id)->SetSourceBreakpoints(sourceBreakpoints);
    }

    if (discoveredPrograms.empty()) {
        statusText = "No child applications were discovered in the target project directory.";
        return;
    }

    statusText = "Loaded " + std::to_string(discoveredPrograms.size()) + " child applications.";
}

void TargetProject::Update() {
    for (auto& program : programs) {
        program.second->Update();
    }
}

void TargetProject::SetSourceBreakpoints(const std::vector<SourceBreakpoint>& breakpoints) {
    sourceBreakpoints = breakpoints;
    for (auto& [id, program] : programs) {
        (void)id;
        program->SetSourceBreakpoints(sourceBreakpoints);
    }
}

const std::vector<SourceBreakpoint>& TargetProject::SourceBreakpoints() const {
    return sourceBreakpoints;
}

TargetProject::Programs& TargetProject::GetPrograms() {
    return programs;
}

const TargetProject::Programs& TargetProject::GetPrograms() const {
    return programs;
}

const std::string& TargetProject::StatusText() const {
    return statusText;
}
