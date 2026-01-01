//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include <string>

#include "ModuleCompiler.hpp"

struct ModuleSettings;

class ModuleDefinition {
public:
    ModuleDefinition(ModuleSettings& settings, const std::string& id, const std::string& mainPath);

    bool LibraryExists() const;

    std::string LibraryPath() const;

    ModuleCompilerResult Build();

private:
    std::string id;
    std::string mainPath;
    ModuleSettings &settings;
    ModuleCompiler compiler;

};
