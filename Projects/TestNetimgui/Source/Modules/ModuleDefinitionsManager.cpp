//
// Created by Jeppe Nielsen on 19/04/2024.
//

#include "ModuleDefinitionsManager.hpp"
#include "ProjectSettings.hpp"

ModuleDefinitionsManager::ModuleDefinitionsManager(ModuleSettings& moduleSettings, ProjectSettings& projectSettings) :
moduleSettings(moduleSettings), projectSettings(projectSettings) {

}

ModuleDefinition& ModuleDefinitionsManager::CreateFromMainFile(const std::string &id, const std::string &mainPath) {
    definitions.insert({id, std::make_unique<ModuleDefinition>(moduleSettings, projectSettings, id, mainPath)});
    return *definitions[id].get();
}

const ModuleDefinitionsManager::DefinitionsList& ModuleDefinitionsManager::Definitions() const {
    return definitions;
}
