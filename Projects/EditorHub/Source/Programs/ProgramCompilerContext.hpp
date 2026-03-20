//
// Created by Jeppe Nielsen on 20/03/2026.
//

#pragma once
#include <string>
#include <vector>

struct ProgramCompilerContext {
    std::vector<std::string> includePaths;
    std::vector<std::string> sourceFiles;
    std::vector<std::string> defines;
    std::vector<std::string> libraryPaths;
    std::vector<std::string> libraries;
    std::vector<std::string> frameworks;
    std::string outputPath;
};
