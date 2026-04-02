//
// Created by Codex on 02/04/2026.
//

#pragma once
#include "../../../EditorHub/Source/Programs/SourceBreakpoint.hpp"
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

class CodeEditorBreakpointOverview {
public:
    struct DrawResult {
        std::string openedPath;
        int openedLine = 0;
    };

    DrawResult Draw(const std::vector<SourceBreakpoint>& breakpoints, const std::string& activePath) const;

private:
    struct CachedFileLines {
        std::filesystem::file_time_type lastWriteTime{};
        bool hasTimestamp = false;
        std::vector<std::string> lines;
    };

    const std::string& GetLinePreview(const std::string& filePath, int line) const;
    void RefreshCachedFile(const std::string& filePath) const;

    mutable std::unordered_map<std::string, CachedFileLines> cachedFileLines;
};
