//
// Created by Jeppe Nielsen on 19/04/2024.
//

#include "ModuleManager.hpp"

ModuleManager::ModuleManager(EngineContext &engineContext) : engineContext(engineContext){

}

void ModuleManager::AddModule(const std::string &moduleId, ModuleDefinition &definition) {
    modules.insert({moduleId, std::make_unique<Module>(engineContext, definition)});
}

ModuleManager::Modules& ModuleManager::GetModules() {
    return modules;
}
