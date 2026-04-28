//
// Created by Codex on 04/04/2026.
//

#pragma once
#include "../../../EditorHub/Source/Project/TargetProject.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>

class CodeEditorProgramOutputWindow {
public:
    void SetDisplayRootPath(std::string rootPath);
    void Draw(TargetProject::Programs& programs);

private:
    std::string displayRootPath;
    std::unordered_map<std::string, std::size_t> lastOutputSizes;
    std::unordered_map<std::string, std::string> outputBuffers;
    bool followOutput = true;
};
