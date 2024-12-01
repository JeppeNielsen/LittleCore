//
// Created by Jeppe Nielsen on 30/11/2024.
//


#pragma once
#include "../Modules/ModuleDefinitionsManager.hpp"
#include "../Modules/ModuleManager.hpp"
#include "../Project/ProjectSettings.hpp"
#include "../ScriptsInclude/EngineContext.hpp"

class Project {
public:

    Project(EngineContext& engineContext);

    ModuleSettings moduleSettings;
    ModuleDefinitionsManager moduleDefinitionsManager;
    ModuleManager moduleManager;
    ProjectSettings projectSettings;

    void Reset();
    void LoadProject(const std::string rootPath);

};
