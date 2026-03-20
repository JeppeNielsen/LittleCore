//
// Created by Jeppe Nielsen on 20/03/2026.
//

#pragma once
#include <map>
#include <memory>
#include <string>
#include "../Programs/Program.hpp"
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

    Programs& GetPrograms();
    const Programs& GetPrograms() const;

    const std::string& StatusText() const;

private:
    TargetProjectSettings settings;
    Programs programs;
    std::string statusText;

    static std::string ResolveWorkspaceRoot();
};
