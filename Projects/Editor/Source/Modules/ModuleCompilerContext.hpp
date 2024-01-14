//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include <string>
#include <vector>

struct ModuleCompilerContext {
    std::vector<std::string> hppFiles;
    std::vector<std::string> cppFiles;
    std::string outputPath;
};
