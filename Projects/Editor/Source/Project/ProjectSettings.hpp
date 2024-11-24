//
// Created by Jeppe Nielsen on 24/11/2024.
//


#pragma once
#include <string>
#include "../Modules/ModuleCompilerContext.hpp"
#include "../Modules/ModuleDefinitionsManager.hpp"

struct ProjectSettings {
    std::string rootPath;
    void ModifyModules(ModuleCompilerContext& moduleCompilerContext, ModuleDefinitionsManager& moduleDefinitionsManager);
};
