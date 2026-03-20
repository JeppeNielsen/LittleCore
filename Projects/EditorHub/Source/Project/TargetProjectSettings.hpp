//
// Created by Jeppe Nielsen on 20/03/2026.
//

#pragma once
#include <string>
#include <vector>

struct ProgramCompilerContext;

struct DiscoveredProgram {
    std::string id;
    std::string stateTypeName;
    std::string sourcePath;
};

class TargetProjectSettings {
public:
    std::string workspaceRoot;
    std::string rootPath;
    std::string engineAssetsPath;
    std::string cachePath;

    void Populate(ProgramCompilerContext& sharedContext, std::vector<DiscoveredProgram>& programs) const;
    bool HasValidRoot() const;
    bool HasValidEngineAssetsPath() const;

private:
    static bool TryParseStateTypeName(const std::string& source, std::string& stateTypeName);
};
