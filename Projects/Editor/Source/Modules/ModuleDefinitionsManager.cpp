//
// Created by Jeppe Nielsen on 19/04/2024.
//

#include "ModuleDefinitionsManager.hpp"

ModuleDefinitionsManager::ModuleDefinitionsManager(ModuleSettings& moduleSettings) : moduleSettings(moduleSettings) {

}

ModuleDefinition& ModuleDefinitionsManager::CreateFromMainFile(const std::string &id, const std::string &mainPath) {
    definitions.insert({id, std::make_unique<ModuleDefinition>(moduleSettings, id, mainPath)});
    return *definitions[id].get();
}
