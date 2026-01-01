//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include <string>
#include <vector>

struct ModuleCompilerContext {
    std::vector<std::string> hppFiles;
    std::vector<std::string> cppFiles;
    std::vector<std::string> defines;
    std::vector<std::string> libPaths;
    std::vector<std::string> libs;
    std::string outputPath;
};
