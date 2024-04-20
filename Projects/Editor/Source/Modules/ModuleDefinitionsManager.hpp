//
// Created by Jeppe Nielsen on 19/04/2024.
//


#pragma once
#include <memory>
#include <map>
#include "ModuleSettings.hpp"
#include "ModuleDefinition.hpp"

class ModuleDefinitionsManager {
public:
    ModuleDefinitionsManager(ModuleSettings& moduleSettings);

    ModuleDefinition& CreateFromMainFile(const std::string& id, const std::string& mainPath);

private:
    ModuleSettings& moduleSettings;
    std::map<std::string, std::unique_ptr<ModuleDefinition>> definitions;

};
