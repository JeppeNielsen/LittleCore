//
// Created by Jeppe Nielsen on 30/11/2024.
//


#pragma once
#include "ModuleDefinitionsManager.hpp"
#include "ModuleManager.hpp"
#include "ProjectSettings.hpp"

class Project {
public:
    Project();
    ModuleSettings moduleSettings;
    ModuleDefinitionsManager moduleDefinitionsManager;
    ModuleManager moduleManager;
    ProjectSettings projectSettings;

    void Reset();
    void LoadProject(const std::string rootPath);

};
