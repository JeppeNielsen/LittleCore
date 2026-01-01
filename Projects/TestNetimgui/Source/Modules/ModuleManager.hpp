//
// Created by Jeppe Nielsen on 19/04/2024.
//


#pragma once
#include <map>
#include <memory>
#include "Module.hpp"

class ModuleManager {
public:
    ModuleManager();

    using Modules = std::map<std::string, std::unique_ptr<Module>>;

    void AddModule(const std::string& moduleId, ModuleDefinition& definition);
    Modules& GetModules();

private:
    Modules modules;

};
