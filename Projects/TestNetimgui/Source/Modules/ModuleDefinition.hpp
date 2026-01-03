//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include <string>

#include "ModuleCompiler.hpp"
#include "TaskRunner.hpp"
#include "Timer.hpp"

struct ModuleSettings;
struct ProjectSettings;

class ModuleDefinition {
public:
    ModuleDefinition(ModuleSettings& settings, ProjectSettings& projectSettings, const std::string& id, const std::string& mainPath);

    bool LibraryExists() const;

    std::string LibraryPath() const;

    void StartBuild();

    bool IsBuilding() const;

    ModuleCompilerResult GetResult() const;

    void Update();

private:
    std::string id;
    std::string mainPath;

    bool isBuilding = false;

    ModuleSettings& settings;
    ProjectSettings& projectSettings;
    ModuleCompiler compiler;

    ModuleCompilerResult result;

    LittleCore::TaskRunner taskRunner;
    LittleCore::Timer compilationTimer;
    std::vector<std::string> errorsFromCompilation;

    std::string CreateMainContent();
    std::string EnsureMainFile();


};
