//
// Created by Codex on 01/04/2026.
//

#pragma once
#include <string>
#include <vector>

struct DebuggerVariable {
    std::string name;
    std::string value;
    std::string type;
    int variablesReference = 0;
};

struct DebuggerScope {
    std::string name;
    bool expensive = false;
    int variablesReference = 0;
    bool variablesLoaded = false;
    std::vector<DebuggerVariable> variables;
};
