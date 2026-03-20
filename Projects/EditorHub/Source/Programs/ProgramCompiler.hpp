//
// Created by Jeppe Nielsen on 20/03/2026.
//

#pragma once
#include <string>
#include "ProgramCompilerContext.hpp"
#include "ProgramCompilerResult.hpp"

class ProgramCompiler {
public:
    explicit ProgramCompiler(std::string clangPath);
    ProgramCompilerResult Compile(const ProgramCompilerContext& context) const;

private:
    std::string clangPath;
};
