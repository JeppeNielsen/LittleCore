//
// Created by Jeppe Nielsen on 30/11/2024.
//


#pragma once
#include "../Modules/ModuleDefinitionsManager.hpp"
#include "../Modules/ModuleManager.hpp"
#include "../Project/ProjectSettings.hpp"
#include "../ScriptsInclude/EngineContext.hpp"
#include "ResourcePathMapper.hpp"
#include "DefaultResourceManager.hpp"

class Project {
public:

    Project(EngineContext& engineContext);

    ModuleSettings moduleSettings;
    ModuleDefinitionsManager moduleDefinitionsManager;
    ModuleManager moduleManager;
    ProjectSettings projectSettings;
    LittleCore::ResourcePathMapper resourcePathMapper;
    LittleCore::DefaultResourceManager defaultResourceManager;

    void Reset();
    void LoadProject(const std::string rootPath);

};
