//
// Created by Jeppe Nielsen on 30/11/2024.
//

#pragma once

#include "CompilationResult.hpp"

struct IProjectCompilerHandler {

    virtual ~IProjectCompilerHandler() = 0;
    virtual void CompilationFinished(CompilationResult& result) = 0;

};