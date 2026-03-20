//
// Created by Jeppe Nielsen on 20/03/2026.
//

#pragma once
#include <string>

struct ProgramCompilerResult {
    bool succeeded = false;
    std::string summary;
    std::string command;
    std::string output;
};
