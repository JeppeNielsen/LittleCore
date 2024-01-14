//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include <future>
#include <string>
#include "ModuleCompilerContext.hpp"
#include "ModuleCompilerResult.hpp"

class ModuleCompiler {
    std::string clangPath;

public:
    ModuleCompiler(const std::string& clangPath);
    ModuleCompilerResult Compile(const ModuleCompilerContext& context) const;
};
