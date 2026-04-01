//
// Created by Codex on 01/04/2026.
//

#pragma once
#include <string>

struct SourceBreakpoint {
    std::string filePath;
    int line = 0;

    auto operator==(const SourceBreakpoint&) const -> bool = default;
};
