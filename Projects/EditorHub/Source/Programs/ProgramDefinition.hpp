//
// Created by Jeppe Nielsen on 20/03/2026.
//

#pragma once
#include <string>
#include "ProgramCompilerContext.hpp"
#include "ProgramCompilerResult.hpp"

struct DiscoveredProgram;

class ProgramDefinition {
public:
    ProgramDefinition(const std::string& rootPath,
                      const std::string& engineAssetsPath,
                      const std::string& cachePath,
                      const ProgramCompilerContext& sharedContext,
                      const DiscoveredProgram& program);

    const std::string& Id() const;
    const std::string& StateTypeName() const;
    const std::string& SourcePath() const;

    std::string GeneratedMainPath() const;
    std::string ExecutablePath() const;
    bool ExecutableExists() const;
    ProgramCompilerResult Build() const;

private:
    std::string rootPath;
    std::string engineAssetsPath;
    std::string cachePath;
    ProgramCompilerContext sharedContext;
    std::string id;
    std::string stateTypeName;
    std::string sourcePath;

    std::string CreateGeneratedMainSource() const;
    void EnsureGeneratedMainFile() const;
};
