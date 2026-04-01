//
// Created by Jeppe Nielsen on 20/03/2026.
//

#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "../Programs/Program.hpp"
#include "../Programs/SourceBreakpoint.hpp"
#include "TargetProjectSettings.hpp"

class TargetProject {
public:
    using Programs = std::map<std::string, std::unique_ptr<Program>>;

    TargetProject();
    ~TargetProject();

    TargetProjectSettings& Settings();
    const TargetProjectSettings& Settings() const;

    void Reload();
    void Update();
    void SetSourceBreakpoints(const std::vector<SourceBreakpoint>& breakpoints);
    const std::vector<SourceBreakpoint>& SourceBreakpoints() const;

    Programs& GetPrograms();
    const Programs& GetPrograms() const;

    const std::string& StatusText() const;

private:
    TargetProjectSettings settings;
    Programs programs;
    std::string statusText;
    std::vector<SourceBreakpoint> sourceBreakpoints;

    static std::string ResolveWorkspaceRoot();
};
