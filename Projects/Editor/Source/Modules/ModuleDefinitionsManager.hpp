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

    using DefinitionsList =  std::map<std::string, std::unique_ptr<ModuleDefinition>>;

    ModuleDefinitionsManager(ModuleSettings& moduleSettings);

    ModuleDefinition& CreateFromMainFile(const std::string& id, const std::string& mainPath);

    const DefinitionsList& Definitions() const;

private:
    ModuleSettings& moduleSettings;
    DefinitionsList definitions;

};
