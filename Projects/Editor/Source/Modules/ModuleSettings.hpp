//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include <string>

#include "ModuleCompiler.hpp"

struct ModuleSettings {
    std::string libraryFolder;
    ModuleCompilerContext context;

    ModuleCompiler CreateCompiler();
};
